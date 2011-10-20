var FGDB = require(__dirname + '/build/default/fgdb.node').FGDB;
var events = require('events');

inherits(FGDB, events.EventEmitter);
exports.FGDB = FGDB;

function inherits(target, source) {
	for (var k in source.prototype) {
		target.prototype[k] = source.prototype[k];
	}
}