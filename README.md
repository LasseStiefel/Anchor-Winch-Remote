# Anker Whinch Remote Control
Small Note: This is my first Public projct on GitHub so some things might be a bit messy 🙃

## Project Outline
The project aims  to create a remote control for the anker winch on a sailing/ motor yacht. 
Retrofitting a system is expensive and I thought this might be a cool little project to pursue.
The Anchor chain I am working with is the "Lofran's Tigres", however this project should work with most of the models and brands.
(Product Example: https://www.lofrans.com/product/75-hand-held-controls/6600-lofrans-radio-control-chaincounter-thetis-7003 - approx. 250 €)

## Goals
- Controling whinch movement (Up/ Down) wirelessly **Achieved**
- Meassuring chain length and displaying it on the remote

## Project Status - 28. Jul. 2024
Currently the remote remote hardware is designed and capable of fullfiling all project goals. 
The Winch is connected to the Double-Relay which is able to comunicate with the remote control and control the winch. 

A chain counting mechanism is not implemented yet.

## Micro Controller and remote
The Micro Conrollers I chose are two "Seed Studio Xiao ESP32C3". These are extremly capable yet light weight controllers.
These Micro Controllers where chosen because they have great connectivity, low energy consumption, Tiny Form factor and a **built in battery charging chip**.
(Product Link: https://www.seeedstudio.com/Seeed-XIAO-ESP32C3-p-5431.html)



