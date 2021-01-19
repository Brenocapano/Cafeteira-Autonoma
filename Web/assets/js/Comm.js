const Comm = {
    ip: "0.0.0.0",
    isConnected: false,
    listnerTimer: null,
    waiting: false,



    init: function(ip){
        Comm.ip = ip;
        Comm.tryConnect();
    },

    updateIp: function(newIp){
        Comm.ip = newIp;
        Comm.tryConnect();
    },

    tryConnect: function(){
        Comm.handshake();
        //Comm listner
        Comm.listner();
    },

    listner: function(){
        clearInterval(Comm.listnerTimer);
        Comm.listnerTimer = setInterval(function(){ 
            if (Comm.waiting){
                return;
            }
            Comm.handshake();
        }, 5000)
    },

    send: function(url, scbk,ecbk){
        if (!Comm.isConnected){
            return;
        }
        Comm.waiting = true;
        $.ajax({
            url: Comm.ip+url,
            method: "GET",
            success: function(data){
                Comm.waiting = false;
                if (typeof scbk === "function"){
                    scbk(data);
                }

            },
            error: function(data){
                Comm.waiting = false;
                if (typeof ecbk === "function"){
                    ecbk(data);
                }
            }
        });
    },

    sendForce: function(url, data,scbk, ecbk){
        Comm.send(url+"?"+data, scbk, ecbk);
    },

    handshake: function(){
        $.ajax({
            url: Comm.ip+"/",
            method: "GET",
            success: function(data){
                if (data === "hello"){
                    Comm.isConnected = true;
                } else {
                    Comm.isConnected = false;
                }
            },
            error: function(data){
                Comm.isConnected = false;
            }
        });
    }
}