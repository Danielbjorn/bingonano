from subprocess import call
import os

def play_flite():
    call(["flite", "-t", "Hello motherfucker"])
