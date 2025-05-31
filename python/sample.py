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

    def __init__(self, standalone: bool = False):
        super().__init__(None, 60)

        size = (320, 200)
        self.surface = pygame.display.set_mode(size) if standalone else pygame.Surface(size, depth=32)

        font = pygame.font.SysFont("Arial", 64, bold=True)
        self.text = font.render("Libretro core in Python. Press Tab key or Left/Right joypad buttons.", True, (192, 96, 96))

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

# Standalone mode

class Game(GameCore):
    joypadMap = {
        pygame.K_LEFT: RetroKey.JOYPAD_LEFT,
        pygame.K_RIGHT: RetroKey.JOYPAD_RIGHT
    }
    keyMap = {
        pygame.K_TAB: RetroKey.RETROK_TAB
    }

    running: bool

    def __init__(self):
        super().__init__(True)

    def run(self):
        self.running = True
        while self.running:
            self.handleEvents()
            self.nextFrame()
            pygame.display.flip()
            time.sleep(1 / self.fps)

    def handleEvents(self):
        for event in pygame.event.get():
            match event.type:
                case pygame.QUIT:
                    self.running = False
                case pygame.KEYDOWN:
                    self.handleKeydown(event.key)

    def handleKeydown(self, key: int):
        if key == pygame.K_ESCAPE:
            self.running = False
            return

        button = self.joypadMap.get(key)
        if button is not None:
            self.joypadEvent(0, button, True)
            return

        key = self.keyMap.get(key)
        if key is not None:
            self.keyboardEvent(key, True, 0, 0)
            return

if __name__ == "__main__":
    game = Game()
    game.run()
