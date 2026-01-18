import React, { useState } from 'react';
import { Button, FlatList, PermissionsAndroid, Text, View } from "react-native";
import RNBluetoothClassic from "react-native-bluetooth-classic";

export default function Index() {

  //let i = 0
async function requestPermission(){
  const granted = await PermissionsAndroid.requestMultiple([
    PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
    PermissionsAndroid.PERMISSIONS.BLUETOOTH_SCAN,
    PermissionsAndroid.PERMISSIONS.BLUETOOTH_CONNECT,
  ])
}


async function scan(){
          const devices = await RNBluetoothClassic.startDiscovery();
          const classicdevs = devices.filter(device => device.type == "DUAL" )
          console.log("FOUND CLASSIC DEVICES:", classicdevs);
          setDevices(classicdevs)
          //return classicdevs
        }

async function connectDevice(device: any){
    //setStatus("active")
    try{
      await device.connect()
      setConnection("successful")
      device.onDataReceived((event: any) => {
      console.log(event.data);
    });
    }
    catch(error){
      setConnection("failed")
    }
    
    
    
    // console.log("You have now connected to your device.")
}


  const [devices, setDevices] = useState<any[]>([]);
  const [noaudio, setnoaudio] = useState(true)
  const [connection, setConnection] = useState("null") //connect is var name and is initalized to connecting..., 
                                                          //update value (connect) using 'setconnect'
  
  return (
    <View style={{ flex: 1, backgroundColor: "white", padding: 60 }}>
      <Button title={noaudio ? "Scan for bluetooth devices" : "Searching..."}
      disabled={false}
        onPress={async () => {
        setnoaudio(false); //sets no audio to false
        await requestPermission()
        await scan()
        }}/>
      <FlatList 
        data = {devices}
        renderItem={({item}) => 
        (
          <Button title={item.name}
           onPress = {async () => {
           await connectDevice(item)
           
           }}/>
          

        )}  />
        <Text>Connection {connection}</Text>


        
        

       
      

    </View>
  );

}