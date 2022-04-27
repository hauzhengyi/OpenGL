@echo off

g++ tunnel.cpp getBMP.cpp -w -fpermissive -lopengl32 -lglu32 -lglew32 -lfreeglut -o output.exe 
output