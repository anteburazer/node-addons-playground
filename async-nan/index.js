const addon = require('./build/Release/primes');

// addon.primesSync(20, function (primes) {
//     // prints 2, 3, 5, 7, 11, 13, 17, 19
//     console.log("Primes sync less than 20 = " + primes);
// });

// addon.primesAsync(20, function (primes) {
//     // prints 2, 3, 5, 7, 11, 13, 17, 19
//     console.log("Primes async less than 20 = " + primes);
// });

addon.primesAsyncProgress(20, (primes) => {
    console.log("Primes less than 20 = " + primes);
}, (progress) => {
    console.log(progress + "% complete");
});