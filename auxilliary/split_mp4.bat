ffmpeg -i explosion_pink.mp4 -vf "fps=8,format=rgba,colorkey=0x515057:0.25:0.25" -frames:v 50 "%%03d.png"
