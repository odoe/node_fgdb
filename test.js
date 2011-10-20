var FGDB = require('./fgdb').FGDB;
var e = new FGDB("data/Querying.gdb");
var table = "Cities";

console.log('set up listener for onAllFields');
e.on('onAllFields', function(r) {
	console.log('Available fields are: %s', r);
});

console.log('launch onAllFields');
e.allFields(table);

console.log('set up listener for onQueryComplete');
e.on('onQueryComplete', function(r) {
	console.log('My query result is: %s', r);
});


var query = "CITY_NAME LIKE 'Port%'";
console.log('launch query');
e.query(table, query);