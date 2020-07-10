var http = require('http');
const mosca = require('mosca');

var flag = true;   //is msg valid
var msg = "none";

//http server
var http_port = 5000;
http.createServer(function(req, res){
    var http_status;
    if(flag)
        http_status = 200;
    else
        http_status = 204;

    res.writeHead(http_status, {'Content-type':'text/plain'});
    res.write(msg);
    res.end();
    msg = "none";
    flag = false;

    console.log('======= http =======');
    console.log('http response status: '+http_status);
    console.log('http response message: '+msg);
    console.log('====================');

}).listen(http_port);
console.log("Http sever is now listening on port " + http_port+' ...');

//mqtt server
const moscaSettings = {
    port: 3000
};

var server = new mosca.Server(moscaSettings);
server.on('ready', ()=>{
    console.log('Mqtt sever is now listening on port '+moscaSettings.port+' ...');
});

server.on('clientConnected', (client)=>{
    console.log(client.id+' is connected');
});

server.on('clientDisconnected', (client)=>{
    console.log(client.id+' is disconnected');
});

server.on('subscribed', (topic, client)=>{
    console.log(client.id+' subscribed to '+topic);
});

server.on('published', (packet, client)=>{
    if(client){
        try{
            console.log('published by '+client.id+' :');
            var json_source = JSON.parse(packet.payload.toString('ASCII'));
            console.log(json_source);
            flag = true;
            msg = packet.payload.toString('ASCII');
        }catch(e){ }
    }
});
