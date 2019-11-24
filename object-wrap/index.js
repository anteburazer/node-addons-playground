const { Polynomial } = require('./build/Release/polynomial');

var poly = new Polynomial(1, 3, 2);
console.log(poly);
console.log(poly.at(4)) // prints 30
console.log(poly.roots()) // prints [ -1, -2 ]

poly.c = 0;

console.log(poly.at(4))  // prints 28 

console.log(poly);  // Polynomial {c:0}