var http = require('http');
var url = require('url');
const mosca = require('mosca');

var flag = false;   //is msg valid
var op = "none";
var regDevices = [];
const place_map = new Map([
    ['廚房',0],
    ['客廳',1],
    ['大門',2],
    ['書房',3],
    ['玄關',4],
    ['餐廳',5]
]);

//http server
var http_port = 5000;
http.createServer(function(req, res){
    var pathname = url.parse(req.url).pathname;

    if(pathname === '/all'){
        console.log('request for registered devices info');
        http_status = 200;
        msg = regDevices.join(',');
    }
    else{
        var http_status;
        if(flag)
            http_status = 200;
        else
            http_status = 204;
        
        msg = op;
        op = "none";
        flag = false;
    }
    res.writeHead(http_status, {'Content-type':'text/plain'});
    res.write(msg);
    res.end();
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
            var json_source = JSON.parse(packet.payload.toString('utf-8'));
            console.log(json_source);
            //target: "deviceID/place@mac"
            if(packet.topic === 'arc'){
                flag = true;
                op = JSON.stringify(JSON.parse(packet.payload.toString('utf-8')));
                if(json_source.action === 'reg'){
                    regDevices.push(json_source.target);
                }
                else if(json_source.action === 'del'){
                    var pos = regDevices.indexOf(json_source.target);
                    regDevices.splice(pos, 1);  //移除該元素
                }
            }
            else if(packet.topic === 'init'){
                var device = json_source.target;
                if(regDevices.indexOf(json_source.target) == -1){
                    regDevices.push(device);
                }
            }
        }catch(e){ }
    }
});
