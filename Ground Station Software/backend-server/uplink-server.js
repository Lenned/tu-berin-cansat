var express = require('express');

function StaticServer(spacecraft) {
    var router = express.Router();

    router.get('/', function(req, res) {
        res.sendFile(__dirname + '/uplink.html');
    });

    router.get('/calireset', function(req, res) {
        spacecraft.uplink('A');
        res.sendFile(__dirname + '/uplink.html');
    });

    router.get('/caliall', function(req, res) {
        spacecraft.uplink('B');
        res.sendFile(__dirname + '/uplink.html');
    });

    router.get('/disableauto', function(req, res) {
        spacecraft.uplink('C');
        res.sendFile(__dirname + '/uplink.html');
    });

    router.get('/arm', function(req, res) {
        spacecraft.uplink('D');
        res.sendFile(__dirname + '/uplink.html');
    });

    router.get('/preflight', function(req, res) {
        spacecraft.uplink('E');
        res.sendFile(__dirname + '/uplink.html');
    });

    router.get('/flight', function(req, res) {
        spacecraft.uplink('F');
        res.sendFile(__dirname + '/uplink.html');
    });

    router.get('/landing', function(req, res) {
        spacecraft.uplink('G');
        res.sendFile(__dirname + '/uplink.html');
    });

    router.get('/postlanding', function(req, res) {
        spacecraft.uplink('H');
        res.sendFile(__dirname + '/uplink.html');
    });

    router.get('/releaseparachute', function(req, res) {
        spacecraft.uplink('I');
        res.sendFile(__dirname + '/uplink.html');
    });

    router.get('/attachparachute', function(req, res) {
        spacecraft.uplink('J');
        res.sendFile(__dirname + '/uplink.html');
    });

    router.get('/motoron', function(req, res) {
        spacecraft.uplink('K');
        res.sendFile(__dirname + '/uplink.html');
    });

    router.get('/motoroff', function(req, res) {
        spacecraft.uplink('L');
        res.sendFile(__dirname + '/uplink.html');
    });

    router.get('/servo0', function(req, res) {
        spacecraft.uplink('M');
        res.sendFile(__dirname + '/uplink.html');
    });

    router.get('/servo45', function(req, res) {
        spacecraft.uplink('N');
        res.sendFile(__dirname + '/uplink.html');
    });

    router.get('/servo95', function(req, res) {
        spacecraft.uplink('O');
        res.sendFile(__dirname + '/uplink.html');
    });

    return router
}

module.exports = StaticServer;
