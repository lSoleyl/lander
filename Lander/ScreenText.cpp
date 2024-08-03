#include "stdafx.h"
#include "ScreenText.hpp"

namespace Lander {

  ScreenText::ScreenText() : textFormat(NULL) {
    size = Size(990, 150);
  }

  void ScreenText::SetRunning() {
    state = GAMESTATE::RUNNING;
  }

  void ScreenText::SetGameOver() {
    state = GAMESTATE::GAMEOVER;
  }

  void ScreenText::SetVictory() {
    state = GAMESTATE::VICTORY;
  }

  void ScreenText::Initialize(Size size) {
    windowSize = size;
    pos += Vector::Down * windowSize.height / 2.7f + Vector::Right * 150;
    position = Vector::Up * 50;
  }

  void ScreenText::Draw(RenderInterface& renderInterface, double secondsPassed) {

    if (!textFormat) {
      textFormat = renderInterface.CreateTextFormat(L"Consolas", 200);
    }
    if (state == GAMESTATE::GAMEOVER) {
      renderInterface.DrawText(defeatString, textFormat, Rectangle(position, size), Color::DarkRed);
      renderInterface.DrawLine(position + Vector::Down * 200, position + Vector::Down * 200 + Vector::Right * 990, Color::DarkRed, 10);
    } else if (state == GAMESTATE::VICTORY) {
      renderInterface.DrawText(victoryString, textFormat, Rectangle(position + Vector::Right * 100, size), Color::DarkGreen);
      renderInterface.DrawLine(position + Vector::Down * 200 + Vector::Right * 100, position + Vector::Down * 200 + Vector::Right * 890, Color::DarkGreen, 10);
    }
  }

}