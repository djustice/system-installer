#!/bin/bash

if [[ $# -ne 2 ]]; then
    echo "Usage: $0 <color-scheme.colors> <hex-color>"
    exit 1
fi

echo "Colorizing color-theme: $1 with: $2"

# Function to convert hex color to RGB
hex_to_rgb() {
	hex="$1"
	r=$(printf "%d" 0x"${hex:0:2}")
	g=$(printf "%d" 0x"${hex:2:2}")
	b=$(printf "%d" 0x"${hex:4:2}")
	echo "$r,$g,$b"
}

# Validate and convert color input
color_hex="$2"
color_rgb=$(hex_to_rgb "${color_hex}")

# ; : 1.0
# , : 0.5
# . : 0.3

colors=("DecorationFocus=;"
        "DecorationHover=;"
        "ForegroundActive=;"
        "ForegroundInactive=."
        "ForegroundLink=;"
        "ForegroundNegative=,"
        "ForegroundNormal=;"
        "ForegroundVisited=,"
        "BackgroundNormal=."
        "DecorationFocus=;"
        "ForegroundInactive=,"
        "activeBlend=."
        "activeForeground=;"
        "inactiveBlend=."
        "inactiveForeground=.")

if [[ -e System-$2.colors ]]; then
    rm System-$2.colors
fi

cat $1 | while read line; do
    result=$line

    for color in "${colors[@]}"; do
        name=$(echo $color | cut -d= -f1)
        if [[ $line == $name* ]]; then
            value=$(echo $line | cut -d= -f2)

            if [[ $color == *\; ]]; then
                if [[ $value == $color_rgb ]]; then
                    echo "already changed"
                    continue
                fi

                result=$(echo $line | sed -e "/$name/c\\$name=$color_rgb" | cut -d= -f1-2)
            fi

            if [[ $color == *\, ]]; then
                half_r=$(( $(echo $color_rgb | cut -d, -f1) / 2))
                half_g=$(( $(echo $color_rgb | cut -d, -f2) / 2))
                half_b=$(( $(echo $color_rgb | cut -d, -f3) / 2))
                result=$(echo $line | sed -e "/$name/c\\$name=$half_r,$half_g,$half_b" | cut -d= -f1-2)
            fi

            if [[ $color == *\. ]]; then
                third_r=$(( $(echo $color_rgb | cut -d. -f1) / 3))
                third_g=$(( $(echo $color_rgb | cut -d. -f2) / 3))
                third_b=$(( $(echo $color_rgb | cut -d. -f3) / 3))
                result=$(echo $line | sed -e "/$name/c\\$name=$third_r,$third_g,$third_b" | cut -d= -f1-2)
            fi
        fi
    done

    echo $result >> System-$2.colors
done
