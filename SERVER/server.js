const mosca = require('mosca');

const moscaSettings = {
    port: 3000
};

var server = new mosca.Server(moscaSettings);
server.on('ready', ()=>{
    console.log('Listening on port '+moscaSettings.port+' ...');
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
            var json_source = JSON.parse(packet.payload.toString('UTF-8'));
            console.log(json_source);
        }catch(e){ }
    }
});
