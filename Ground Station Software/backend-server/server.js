/**
 * Basic implementation of a history and realtime server.
 */

var Spacecraft = require('./spacecraft');
var RealtimeServer = require('./realtime-server');
var HistoryServer = require('./history-server');
var StaticServer = require('./static-server');
var UplinkServer = require('./uplink-server');

var expressWs = require('express-ws');
var app = require('express')();
expressWs(app);

var spacecraft = new Spacecraft();
var realtimeServer = new RealtimeServer(spacecraft);
var historyServer = new HistoryServer(spacecraft);
var staticServer = new StaticServer();
var uplinkServer = new UplinkServer(spacecraft);

app.use('/realtime', realtimeServer);
app.use('/history', historyServer);
app.use('/uplink', uplinkServer);
app.use('/', staticServer);

var port = process.env.PORT || 8080

app.listen(port, function () {
    console.log('Open MCT hosted at http://localhost:' + port);
    console.log('History hosted at http://localhost:' + port + '/history');
    console.log('Realtime hosted at ws://localhost:' + port + '/realtime');
    console.log('Uplink hosted at http://localhost:' + port + '/uplink');
});
