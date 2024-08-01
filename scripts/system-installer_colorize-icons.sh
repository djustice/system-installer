#!/bin/bash

##
## made for candy-icons (https://github.com/EliverLara/candy-icons)
##

display_usage() {
	echo "Usage: $0 <iconset-path> <hex-color>"
}

# Check for help option
if [ "$1" == "--help" ] || [ "$1" == "-h" ]; then
	display_usage
	exit 0
fi

# Check for correct number of arguments
if [ $# -ne 2 ]; then
	echo "Error: must input an icon set path and a color"
	display_usage
	exit 1
fi

# Remove trailing slash from iconset path if present
iconset="$1"
iconset=${iconset%%/}

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
name="system-${iconset}-${color_hex}"  # Extract iconset directory name

echo "Converting icon set '$iconset' to color '$color_rgb'"

# Create working directory
mkdir -p "${name}.work"

# List of paths to process
paths=("apps/scalable"
	"categories/scalable"
	"devices/scalable"
	"emblems/8"
	"emblems/16"
	"emblems/22"
	"emblems/symbolic"
	"places/16"
	"places/32"
	"places/48"
	"mimetypes/scalable"
	"preferences/scalable"
	"status/scalable")

# Copy and modify SVG files
for path in "${paths[@]}"; do
	mkdir -p "${name}/${path}"  # Create destination directory

	# Copy SVG files from iconset directory to working directory
	cp "${iconset}/${path}/"*.svg "${name}.work/"

	# Process each copied SVG file in working directory
	for icon in "${name}.work"/*.svg; do
		# Replace color definitions in SVG files
		sed -i -E "s/rgb\(([[:space:]]*[0-9]+[[:space:]]*,?[[:space:]]*[0-9]+[[:space:]]*,?[[:space:]]*[0-9]+[[:space:]]*)\)/${color_rgb}/g" "$icon"
		sed -i -E "s/#[0-9a-fA-F]{6}/${color_hex}/g" "$icon"
		sed -i -E "s/url\([^)]*\)/rgb(${color_rgb})/g" "$icon"

		# Move modified SVG files to final destination
		mv "$icon" "${name}/${path}/" -v
	done
done

# Clean up working directory
rm -r "${name}.work"

# Add index.theme
cp "${iconset}/index.theme" "${name}/index.theme"
sed -i -E "/^Name=/s/.*/Name=System\ Candy\ $2/" "${name}/index.theme"
sed -i -E "/^Comment=/s/.*/Comment=System\ Candy\ $2/" "${name}/index.theme"

echo "Conversion complete. Output directory: $name"
