const addon = require('./build/Release/object_manipulation');

var target = { x: 3 };

console.log('Before: ', target);

addon.increment(target);

console.log('After: ', target);