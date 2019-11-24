"use strict"; 

const streamingWorker = require("streaming-worker");
const path = require("path");

const addon_path = path.join(__dirname, "build/Release/accumulate");
const accumulator = streamingWorker(addon_path);

// accumulator.to.emit("value", 3);
// accumulator.to.emit("value", 16);
// accumulator.to.emit("value", 42);
// accumulator.to.emit("value", -1);

// accumulator.from.on('sum', function(value){
//     console.log("Accumulated Sum:  " + value);
// });



const streamify = require('stream-array');

const input = accumulator.to.stream("value",
    function () {
        accumulator.to.emit('value', -1);
    });

streamify([1, 2, 3, 4, 5, 6]).pipe(input);

accumulator.from.on('sum', function(value){
    console.log("Accumulated Sum:  " + value);
});