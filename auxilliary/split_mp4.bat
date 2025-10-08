ffmpeg -i spit.mp4 -vf "fps=8,format=rgba,colorkey=0x08FF00:0.25:0.25" -frames:v 24 "%%03d.png"
