var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

// Init WebSocket when the page loads
window.addEventListener('load', onload);

function onload() {
  initWebSocket();
  initButtons();
}

function initWebSocket() {
  console.log('Trying to open a WebSocket connection…');
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}

function onOpen() {
  console.log('WebSocket connection opened');
  getReadings();
}

function onClose() {
  console.log('WebSocket connection closed');
  setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
  console.log(event.data);
  var myObj = JSON.parse(event.data);
  var keys = Object.keys(myObj);

  // Update sensor readings
  keys.forEach((key) => {
    if (document.getElementById(key)) {
      document.getElementById(key).innerHTML = myObj[key];
    }
  });

  // Update button states
  if (myObj.output26) {
    updateButtonState("gpio26", myObj.output26);
  }

}

function getReadings() {
  websocket.send("getReadings");
}

function initButtons() {
  document.getElementById("gpio26").addEventListener("click", () => toggleGPIO(26));
}

function toggleGPIO(gpio) {
  websocket.send(`toggle${gpio}`);
}

function updateButtonState(buttonId, state) {
  const button = document.getElementById(buttonId);
  if (state === "on") {
    button.innerHTML = `${buttonId.toUpperCase()} ON`;
    button.classList.remove("off");
  } else {
    button.innerHTML = `${buttonId.toUpperCase()} OFF`;
    button.classList.add("off");
  }
}
