const addon = require('./build/Release/basic_nan');

const numberVal = addon.passNumber(1);
console.log('Number: ', numberVal);

const integerVal = addon.passInteger(1);
console.log('Integer: ', integerVal);

const booleanVal = addon.passBoolean(true);
console.log('Boolean: ', booleanVal);

const stringVal = addon.passString('hello');
console.log('String: ', stringVal);

const objectVal = addon.passObject({ x: 1, y: 2 });
console.log('Object: ', objectVal);

const arrayVal = addon.passArray([1, 2, 'not_index']);
console.log('Array: ', arrayVal);