#pragma once

namespace Lander {

  class ScreenText : public ViewObject {
  public:
    ScreenText();

    void SetRunning();
    void SetGameOver();
    void SetVictory();

    virtual void Initialize(Size size) override;

    virtual void Draw(RenderInterface& renderInterface, double secondsPassed) override;

  private:

    RenderInterface::TextFormat textFormat; //The format used, to write the text

    Vector position;
    Size windowSize;

    std::wstring victoryString = (L"VICTORY");
    std::wstring defeatString = (L"GAME OVER");

    enum class GAMESTATE {RUNNING, GAMEOVER, VICTORY};
    GAMESTATE state = GAMESTATE::RUNNING;
  };

}