import React, { useState } from 'react';
import { Button, PermissionsAndroid, View } from "react-native";
import RNBluetoothClassic from "react-native-bluetooth-classic";

export default function Index() {

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
          return classicdevs
        }



  const [noaudio, setnoaudio] = useState(true)
  
  return (
    <View style={{ flex: 1, backgroundColor: "white", padding: 60 }}>
      <Button title={noaudio ? "Scan for bluetooth devices" : "Searching..."}
      disabled={false}
        onPress={async () => {
        setnoaudio(false); //sets no audio to false
        await requestPermission()
        await scan()
        
        }}/>

        
        

       
      

    </View>
  );
}
