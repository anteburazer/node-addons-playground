// const EventEmitter = require('events');
const path = require('path');
const streamingWorker = require("streaming-worker");

// const makeFactorizer = (n) => {
//     const addon_path = path.join(__dirname, "build/Release/factorization");
//     const worker = require(addon_path);
//     const emitter = new EventEmitter();

//     worker.factorize({n}, () => {
//         emitter.emit("close");
//     }, (event, value) => {
//         emitter.emit(event, value);
//     }, error => {
//         emitter.emit("error", error);
//     });

//     return emitter;
// };

// const addon = makeFactorizer(9007199254740991);

// addon.on('factor', (factor) => {
//     console.log("Factor:  " + factor);
// });

// addon.on('close', () => {
//     console.log("Factorization is complete");
// })

// addon.on('error', (e) => {
//     console.log(e);
// });


/*
 * Factorization wrapped
 */
var addon_path = path.join(__dirname, "build/Release/factorization");
const factorizer = streamingWorker(addon_path, {n: 9007199254740991});

factorizer.from.on('factor', function(factor){
    console.log("Factor:  " + factor);
});

factorizer.from.on('close', function() {
    console.log('closed');
});

factorizer.from.on('error', function(e) {
    console.log('error', e);
});