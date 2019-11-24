"use strict"; 

const streamingWorker = require("streaming-worker");
const path = require("path");
const JSONStream = require('JSONStream');
const fs = require('fs');

const addon_path = path.join(__dirname, "build/Release/sensor_simulator");
const sensor = streamingWorker(addon_path, { name: 'Sensor' });


/*
 * Event emiter
 */
// sensor.from.on('positionSample', function(sample) {
//     console.log(JSON.parse(sample));
// });

/*
 * Streams
 */
const wstream = fs.createWriteStream('positions.log');
const out = sensor.from.stream();
out.pipe(JSONStream.stringify()).pipe(wstream);

setTimeout(() => sensor.close(), 5000);