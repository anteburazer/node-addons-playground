const emitStream = require('emit-stream');
const through = require('through');
const EventEmitter = require('events');

/*
 * Event emiter
 */
// module.exports = function streamingWorker(cppEntryPoint, opts) {
//     const { StreamingWorker} = require(cppEntryPoint);
//     const emitter = new EventEmitter();
//     const worker = new StreamingWorker((event, value) => {
//         emitter.emit(event, value);
//     }, () => {
//         emitter.emit("close");
//     }, error => {
//         emitter.emit("error", error);
//     }, opts);

//     return {
//         from: emitter
//     };
// }

/*
 * Streams
 */
module.exports = function streamingWorker(cppEntryPoint, opts) {
    const { StreamingWorker} = require(cppEntryPoint);
    const emitter = new EventEmitter();
    const worker = new StreamingWorker((event, value) => {
        emitter.emit(event, value);
    }, () => {
        emitter.emit("close");
    }, error => {
        emitter.emit("error", error);
    }, opts);

    const retval = {
        from: emitter,
        close: () => worker.closeInput()
    };

    retval.from.stream = () => {
        return emitStream(retval.from).pipe(
            through(function (data) {
                if (data[0] == "close") {
                    this.end();
                } else {
                    const event = data[0];
                    const position = data[1];

                    console.log(data);
                    this.queue(position);
                }
            })
        );
    }

    // Create a "to" object to similate an emitter
    retval.to = {
        emit: (name, data) => {
            worker.sendToAddon(name, data);   
        },

        stream : function(name, end) {
            var input = through(function write(data) {
                if (Array.isArray(data)) {
                    if ( data[0] == "close"){
                        this.end();
                    } else {
                        retval.to.emit(data[0], data[1]);
                    }
                } else {
                    retval.to.emit(name, data);
                }
            }, end);

            return input;
        }
    }
 
    return retval;
}