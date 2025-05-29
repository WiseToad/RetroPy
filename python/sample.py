#!/usr/bin/env python3

import pygame
import time

from retrocore import RetroCore
from retrokey import RetroKey

# Libretro core

def getCore():
    return GameCore()

class GameCore(RetroCore):
    text: pygame.Surface
    offset: int
    delta: int

    def __init__(self):
        super().__init__(320, 200, 60)
        font = pygame.font.SysFont("Arial", 64, bold=True)
        self.text = font.render("Libretro core in Python. Press Tab key or Left or Right joypad buttons.", True, (192, 96, 96))
        self.offset = self.width // 2
        self.delta = -4

    def nextFrame(self) -> pygame.BufferProxy:
        self.surface.fill((0, 0, 64))
        self.drawText(self.offset)
        self.offset = (self.offset + self.delta) % (self.text.get_width() + self.width // 2)
        return super().nextFrame()

    def drawText(self, offset: int):
        if offset < self.width:
            self.surface.blit(self.text, (offset, (self.height - self.text.get_height()) // 2))
        if offset >= self.width // 2:
            self.drawText(offset - self.width // 2 - self.text.get_width())

    def joypadEvent(self, num: int, button: int, pressed: bool):
        if pressed:
            match button:
                case RetroKey.JOYPAD_LEFT:
                    self.delta = -abs(self.delta)
                case RetroKey.JOYPAD_RIGHT:
                    self.delta = abs(self.delta)

    def keyboardEvent(self, keycode: int, pressed: bool, character: int, modifiers: int):
        if pressed:
            match keycode:
                case RetroKey.RETROK_TAB:
                    self.delta = -self.delta

# Standalone launch

class Game(GameCore):
    keyMap = {
        pygame.K_TAB: RetroKey.RETROK_TAB
    }

    running: bool

    def run(self):
        print(f"Starting demo")
        size = (self.width, self.height)
        screen = pygame.display.set_mode(size)

        self.running = True
        while self.running:
            self.handleEvents()
            self.nextFrame()
            screen.blit(self.surface, (0, 0))
            pygame.display.flip()
            time.sleep(1 / self.fps)

    def handleEvents(self):
        for event in pygame.event.get():
            match event.type:
                case pygame.QUIT:
                    self.running = False
                case pygame.KEYDOWN:
                    try:
                        self.keyboardEvent(self.keyMap[event.key], True, 0, 0)
                    except KeyError:
                        pass

if __name__ == "__main__":
    game = Game()
    game.run()
