ffmpeg -i flame_01.mp4 -vf "fps=8,format=rgba,colorkey=0xFFFFFF:0.25:0.25" -frames:v 24 "%%03d.png"
