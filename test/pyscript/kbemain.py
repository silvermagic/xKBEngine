# -*- coding: utf-8 -*-
import KBEngine

def main():
    print("Run in main")
    print(dir(KBEngine))
    char = KBEngine.world.createCharacter("fwd", 27)
    char.writeDB()
