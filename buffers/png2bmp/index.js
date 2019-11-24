'use strict';

const fs = require('fs');
const path = require('path');
const png2bmp = require('./build/Release/png2bmp');

// reads the input (png) filename from the command line arguments
var png_file = process.argv[2];
var bmp_file = path.basename(png_file, '.png') + ".bmp";
var png_buffer = fs.readFileSync(png_file);

/*
 * Encode PNG image to BMP and save it to a file. Node addon (C++) will do the encoding and saving work
 */
// png2bmp.saveBMP(bmp_file, png_buffer, png_buffer.length);

/*
 * Encode PNG image to BMP in C++ and return the buffer to JavaScript
 */
// const bmp_buffer = png2bmp.getBMP(png_buffer, png_buffer.length);
// fs.writeFileSync(bmp_file, bmp_buffer);

/*
 * Encode PNG image to BMP in C++ and return the buffer to JavaScript, but asynchronously
 */
png2bmp.getBMPAsync(
    png_buffer,
    png_buffer.length,
    bmp_buffer => fs.writeFileSync(bmp_file, bmp_buffer)
);