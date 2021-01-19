const Data = {
    storage: null,
    init: function(data){
        Data.storage = data;
    }, 

    insert: function(key, data){
        if (!Data.exists(key)){
            Data.storage.setItem(key, data);
            return true;
        }
        return false;
    },

    get: function(key){
        if (Data.exists(key)){
            return Data.storage.getItem(key);
        }
        return false;
    },

    inputOrUpdate: function(key, value){
        if (Data.exists(key)){
            return Data.update(key,value);
        }
        return Data.insert(key,value);
    },
    
    update: function(key, value){
        if (Data.exists(key)){
            Data.storage.setItem(key, value);
            return true;   
        }
        return false;
    },

    exists: function(key){
        if (Data.storage.getItem(key) === null){
            return false;
        }
        return true;
    },

}