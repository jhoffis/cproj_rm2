
#!/usr/bin/env bash

project_dir="$HOME/dev/cproj_rm2"
cd "$project_dir" || { echo "Could not cd to $project_dir"; exit 1; }

# Launch a window for compiling
konsole --separate \
        --workdir "$project_dir" \
        --geometry 800x480 \
        --title "Build" &

# Make this window fullscreen
if command -v xdotool >/dev/null; then
    xdotool getactivewindow key F11
fi

nohup flatpak run --branch=stable --arch=x86_64 --command=gittyup com.github.Murmele.Gittyup >/dev/null 2>&1 &

# Start vim with a vertical split
~/nvim-linux64/bin/nvim -c vsplit .

