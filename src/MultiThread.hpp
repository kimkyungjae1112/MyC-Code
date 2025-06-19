#include <algorithm>
#include <chrono>
#include <conio.h>              // _kbhit(), _getch()
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <vector>
#include <ranges>

using namespace std::chrono_literals;

namespace FallingStar
{


struct Block { int X, Y; };

struct GameState
{
    int  W = 40, H = 20;
    int  PlayerX = W / 2;
    std::atomic_bool Running = true;
    std::vector<Block> Blocks;
    int  Score = 0;
} GS;

std::mutex Mtx;
std::condition_variable TickCv;

/* ---------------- Input ---------------- */
void InputLoop()
{
    while (GS.Running.load(std::memory_order_relaxed))
    {
        if (_kbhit())
        {
            int ch = _getch();
            if (ch == 27)          // ESC
            {
                GS.Running.store(false, std::memory_order_relaxed);
                break;
            }
            // 화살표 키는 prefix(0 또는 224) + 실제코드
            if (ch == 0 || ch == 224) ch = _getch();

            if (ch == 75)          // ←
            {
                std::lock_guard lg(Mtx);
                if (GS.PlayerX > 1) --GS.PlayerX;
            }
            else if (ch == 77)     // →
            {
                std::lock_guard lg(Mtx);
                if (GS.PlayerX < GS.W - 2) ++GS.PlayerX;
            }
        }
        std::this_thread::sleep_for(1ms);
    }
}

/* ---------------- Logic ---------------- */
void LogicLoop()
{
    std::mt19937 Rng(std::random_device{}());
    std::uniform_int_distribution<int> Dist(1, GS.W - 2);
    constexpr auto LogicTick = 33ms;   // ≒30 FPS

    while (GS.Running.load(std::memory_order_relaxed))
    {
        auto Wake = std::chrono::steady_clock::now() + LogicTick;

        {
            std::lock_guard lg(Mtx);

            /* ① 새 블록 랜덤 생성 */
            if (Dist(Rng) % 5 == 0)
                GS.Blocks.push_back({ Dist(Rng), 1 });

            /* ② 블록 이동 */
            for (auto& b : GS.Blocks) ++b.Y;

            /* ③ 충돌 검사 */
            if (std::ranges::any_of(GS.Blocks, [](const Block& b)
                {
                    return b.Y == GS.H - 2 && b.X == GS.PlayerX;
                }))
            {
                GS.Running.store(false, std::memory_order_relaxed);
            }

            /* ④ 화면 밖 블록 정리 + 점수 */
            auto it = std::remove_if(GS.Blocks.begin(), GS.Blocks.end(),
                [&](const Block& b) { return b.Y >= GS.H - 1; });
            GS.Score += static_cast<int>(std::distance(it, GS.Blocks.end()));
            GS.Blocks.erase(it, GS.Blocks.end());
        }

        TickCv.notify_one();
        std::this_thread::sleep_until(Wake);
    }
}

/* ---------------- Render ---------------- */
void RenderLoop()
{
    while (GS.Running.load(std::memory_order_relaxed))
    {
        std::unique_lock ul(Mtx);
        TickCv.wait(ul);

        system("cls");
        std::vector<std::string> Buf(GS.H, std::string(GS.W, ' '));

        /* 하단 벽 + 플레이어 */
        Buf[GS.H - 1].assign(GS.W, '#');
        Buf[GS.H - 2][GS.PlayerX] = 'P';

        /* 블록 그리기 */
        for (auto& b : GS.Blocks)
            if (b.Y < GS.H - 1) Buf[b.Y][b.X] = '*';

        for (auto& row : Buf) std::cout << row << '\n';
        std::cout << "Score: " << GS.Score << "\nESC to quit.\n";
    }
}

/* ---------------- Main ---------------- */
void Main()
{
    std::jthread Logic(LogicLoop);
    std::jthread Input(InputLoop);
    RenderLoop();                       // 메인 스레드에서 렌더 담당
    std::cout << "\nGame Over!  Final Score = " << GS.Score << '\n';
}

}