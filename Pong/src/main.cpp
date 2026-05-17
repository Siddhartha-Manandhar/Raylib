#include <raylib.h>
#include <iostream>
#include <cmath>

using namespace std;

Color Green = Color{38, 185, 154, 255};
Color Dark_Green = Color{20, 160, 133, 255};
Color Light_Green = Color{129, 204, 184, 255};
Color Yellow = Color{243, 213, 91, 255};

const int screen_width = 1280;
const int screen_height = 800;

Vector2 mouse_position;

int player_score = 10;
int cpu_score = 0;

bool game_start = false;

class Ball;
class Paddle;
class CpuPaddle;
class Menu;
class Score;

class Ball{
    public:
        float x, y;
        int speed_x, speed_y;
        int radius;
        Sound hit_sound;
        Sound score_update;

        void Draw(){
           DrawCircle(x, y, radius, Yellow); 
        }
        void Update(){
            x += speed_x;
            y += speed_y;

            if (y + radius >= screen_height || y - radius <= 0){
                speed_y *= -1;
                PlaySound(hit_sound);
            }

            if (x + radius >= screen_width){
                cpu_score++;
                PlaySound(score_update);
                Resetball();

            } 
            if (x - radius <= 0){
                player_score++;
                PlaySound(score_update);
                Resetball();
            }
        }

        void Resetball(){
            x = screen_width / 2;
            y = screen_height / 2;

            int speed_choices[4] = {-1,1};
            speed_x *= speed_choices[GetRandomValue(0, 1)] ;
            speed_y *= speed_choices[GetRandomValue(0, 1)] ;
        }
}ball;



class Paddle{
    protected:
        void LimitMovement(){
            if (y <= 0){
                    y = 0;
                }
            if (y + height >= screen_height){
                y = screen_height - height;
            }
        }

    public:
        float x, y;
        float width, height;
        int speed;

        void Draw(){
            DrawRectangleRounded(Rectangle{ x, y, width, height }, 0.8, 0, WHITE);
        }
        void Update(){
            if (IsKeyDown(KEY_UP)){
                y -= speed;
            }
            if (IsKeyDown(KEY_DOWN)){
                y += speed;
            }
            LimitMovement();
        }
}player;

class CpuPaddle : public Paddle{
    public:
        void Update(int ball_y){
            if (y + height / 2 > ball_y){
                y -= speed;
            }
            if (y + height / 2 < ball_y){
                y += speed;
            }
            LimitMovement();
        }
}cpu;

class Menu: public Ball{
    public:
        Sound game_start_sound;


        void LoadSounds(){
            game_start_sound = LoadSound("assets/sounds/game_start.wav");
        }
        void RestartButton(){
            DrawRectangle(screen_width / 2 - 150, screen_height / 2 + 50, 300, 75, WHITE);
            DrawText("Restart", screen_width / 2 - 90, screen_height / 2 + 60, 50, Dark_Green);

            if (CheckCollisionPointRec(mouse_position, Rectangle{screen_width / 2 - 150, screen_height / 2 + 50, 300, 75})){
                DrawRectangle(screen_width / 2 - 160, screen_height / 2 + 45, 320, 95, WHITE);
                DrawText("Restart", screen_width / 2 - 90, screen_height / 2 + 70, 50, Dark_Green);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                    player_score = 0;
                    cpu_score = 0;

                    ball.Resetball();
                    ball.speed_x = 7;
                    ball.speed_y = 7;
                }
            }
        }

        void MainMenu(){
            ClearBackground(Dark_Green);
            
            DrawText("Pong Game", screen_width / 2 - 180, screen_height / 2 - 80, 80, WHITE);

            float t = fmodf(GetTime(), 4.0f);   // 4s cycle
            bool show = (t < 3.0f);             // on for 3s, off for 1s

            float phase = t / 3.0f;             // 0..1 while visible
            float alpha = show ? (0.2f + 0.8f * sinf(phase * 3.14159f)) : 0.0f;
            
            Color text_color = ColorAlpha(WHITE, alpha);
            DrawText("Click to Start", screen_width / 2 - 150, screen_height / 2 + 50, 50, text_color);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                game_start = true;
                PlaySound(game_start_sound);
                player_score = 0;
                cpu_score = 0;

                ball.Resetball();
                ball.speed_x = 7;
                ball.speed_y = 7;
            }

        }
        
};
class Score:public Menu{
    public:   
        void Draw(){
            DrawText (TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
            DrawText (TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);
        }
        void Victory_Message(){
            if (player_score == 10){
                DrawText("You Win!", screen_width / 2 - 150, screen_height / 2 - 40, 80, WHITE);
            }
            if (cpu_score == 10){
                DrawText("CPU Wins!", screen_width / 2 - 200, screen_height / 2 - 40, 80, WHITE);
            }
            RestartButton();

        }
}score;


int main() 
{
    cout << "Starting the game" << endl;

    InitWindow(screen_width,  screen_height, "My Pong Game!");
    InitAudioDevice();
    SetTargetFPS(60);

    ball.radius = 20;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = 7;
    ball.speed_y = 7;
    ball.hit_sound = LoadSound("assets/sounds/hit.wav");
    ball.score_update = LoadSound("assets/sounds/score_update.wav");

    player.width = 25;
    player.height = 120;
    player.x = screen_width - player.width - 10;
    player.y = screen_height / 2 - player.height / 2;
    player.speed = 6;

    cpu.width = 25;
    cpu.height = 120;
    cpu.x = 10;
    cpu.y = screen_height / 2 - cpu.height / 2;
    cpu.speed = 6;

    score.LoadSounds();
    
    while(WindowShouldClose() == false){
        BeginDrawing();

        mouse_position = GetMousePosition();

        if(!game_start){
            score.MainMenu();
        }
        else{
            ball.Update();
            player.Update();
            cpu.Update(ball.y);

            if(CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height})){
                ball.speed_x *= -1;
                PlaySound(ball.hit_sound);
            }

            if(CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu.x, cpu.y, cpu.width, cpu.height})){
                ball.speed_x *= -1;
                PlaySound(ball.hit_sound);
            }

            ClearBackground(Dark_Green);
            DrawRectangle(screen_width / 2, 0, screen_width, screen_height, Green);
            DrawCircle(screen_width / 2, screen_height / 2, 150, Light_Green);
            DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
            ball.Draw();
            player.Draw();
            cpu.Draw();
            score.Draw();
            if(cpu_score == 10 || player_score == 10){
                ball.Resetball();
                ball.speed_x = 0;
                ball.speed_y = 0;
                score.Victory_Message();
            }
        }   
        EndDrawing();

    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}