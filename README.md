# RoadSentinel 🚗👁️
> **An Anti-Sleep Driver Alert System built with Arduino Pro Mini.**

RoadSentinel is a wearable safety device that monitors a driver's eyes using an IR sensor. If the eyes remain closed for more than 2 seconds, it triggers simultaneous haptic (vibration motor) and auditory (buzzer) alerts to wake the driver before a fatigue-related accident occurs.

---

## 📖 What Is This Project?

RoadSentinel is a **drowsiness / micro-sleep detection system** designed for drivers. It is built around a low-cost, low-power **Arduino Pro Mini** and an **infrared (IR) sensor module** worn on glasses or a headband pointed at the driver's eye.

The core idea: when a person starts falling asleep, their eyelids close for a sustained period. RoadSentinel detects this eyelid closure and sounds the alarm.

### The Problem
* Driver fatigue causes thousands of accidents worldwide each year.
* Traditional solutions (cameras + vision AI) are expensive and complex to deploy.
* RoadSentinel offers a simple, cheap, wearable alternative: **no camera, no machine learning — just physics and a microcontroller.**

### The Solution
* A **reflective IR sensor** shines infrared light at the eye and measures how much is reflected.
  * **Eye open:** the eyeball reflects most IR light back to the sensor → sensor output goes `LOW`.
  * **Eye closed:** the eyelid absorbs and scatters the IR light → much less reflection → sensor output goes `HIGH`.
* The Arduino monitors the sensor output with a **non-blocking timer**.
* If the sensor stays `HIGH` (eyes closed) for **2 seconds or more**, the Arduino switches on a **buzzer** and a **vibration motor** through a transistor, waking the driver.

---

## 🧠 How the Firmware Works

The code lives in [`src/RoadSentinel.ino`](src/RoadSentinel.ino). Everything runs in `loop()`, which executes continuously:

```
loop()
 │
 ├─ Read IR sensor (digitalRead)
 │
 ├─ If sensor == HIGH (EYE CLOSED):
 │     │
 │     ├─ First time closed? → record eyeClosedStartTime = millis()
 │     └─ Else if (millis() - startTime) ≥ 2000ms → trigger ALERT
 │
 └─ If sensor == LOW (EYE OPEN):
       → reset timer + turn alert OFF
```

### Key logic points

| Concept | Detail |
| :--- | :--- |
| **Non-blocking timing** | Uses `millis()` instead of `delay()`, so the system always reads the sensor in real time and never freezes. |
| `irSensorPin` | Set to `A1` — Arduino Pro Mini analog pin used as a digital input for the sensor's digital `HIGH`/`LOW` output. |
| `alertPin` | Set to `D3` — drives the BC547 transistor that switches the buzzer + motor. |
| `sleepThreshold` | `2000` ms = the drowsiness threshold. The eyes must stay closed this long before an alert fires. |
| `eyeClosedStartTime` | Timestamp captured when the eye *first* closes. Used to measure closure duration. |
| `isEyeClosed` | State flag that prevents the timer from resetting every loop while the eyes remain closed. |
| Hysteresis | The alert stays `HIGH` as long as the eye stays closed past the threshold; opening the eyes instantly clears it. |

### Alert condition (simplified)

```
eyes closed continuously for ≥ 2 s
        ⇓
digitalWrite(alertPin, HIGH)
        ⇓
BC547 transistor conducts (saturated)
        ⇓
Buzzer sounds + Vibration motor vibrates  ←  "ALERT: Drowsiness Detected!"
```

---

## ✨ Features

* 🔵 **Real-time eye monitoring** via reflective IR sensor.
* ⏱️ **Non-blocking** `millis()` timing — no `delay()` stalls.
* 🚨 **Dual alert output**: audible buzzer + haptic vibration motor.
* 🔄 **Automatic reset** when the driver opens their eyes.
* 🔋 **Battery powered** (3.7V LiPo) for portable, wearable use.
* 💰 **Ultra low cost** — a handful of common electronic components.

---

## 🧰 Hardware Architecture & Bill of Materials

| # | Component | Purpose |
| :--- | :--- | :--- |
| 1 | Arduino Pro Mini (3.3V/5V) | Brain of the system; reads sensor and drives alerts. |
| 2 | 6-Pin CP2102 USB → TTL Module | Programmer used to upload firmware to the Pro Mini. |
| 3 | IR Sensor Module (eye tracking) | Detects eyelid closure via reflected infrared light. |
| 4 | 3.7V LiPo Battery | Powers the system directly (`VCC`). |
| 5 | BC547 NPN Transistor | Low-power switch that lets the Arduino control the buzzer + motor. |
| 6 | 1kΩ Resistor | Base-current limiter for the BC547 transistor. |
| 7 | 5V Buzzer | Auditory alarm. |
| 8 | Vibration Motor | Haptic alarm. |
| 9 | 1N4007 Flyback Diode | Protects the transistor from inductive back-EMF spikes from the motor/buzzer. |

> ⚠️ The flyback diode must be connected **across the motor/buzzer terminals** (cathode to +, anode to −), pointing backward relative to current flow, to clamp voltage spikes when the coil de-energizes.

---

## 🔌 Circuit Connections

| Component | Component Pin | Arduino Pro Mini Pin |
| :--- | :--- | :--- |
| **IR Sensor** | OUT / Signal | `A1` |
| **Transistor** | Base (via 1kΩ Res) | `D3` |
| **Alerts** | Negative Terminal | Transistor Collector |
| **Battery** | Positive | `VCC` |
| **Battery** | Negative | `GND` |

### Understanding the drive stage

The Arduino's `D3` pin can only source a few milliamps, which is not enough to drive a buzzer + motor directly. The **BC547 transistor** acts as a switch:

```
D3 ──── 1kΩ ──── BC547 Base
                    │
Battery + ──── Buzzer/Motor ──── Transistor Collector
                    │
                    Transistor Emitter ──── GND
```

* `D3 HIGH` → base current flows → transistor turns on → collector-emitter path conducts → buzzer & motor are connected to GND → **alarm on**.
* `D3 LOW` → transistor off → current path broken → **alarm off**.

---

## 🛠️ Assembly Steps

1. **Wearable mount**: Attach the IR sensor module to glasses frames or a headband, aiming it at the driver's eye from a short distance (a few centimeters).
2. **Wire the IR sensor**: Connect `OUT` → `A1`, power to `VCC`/`GND`.
3. **Build the transistor switch**: Connect `D3` → `1kΩ` → BC547 base; connect the buzzer & vibration motor (in parallel) from battery `+` to the transistor collector.
4. **Add the flyback diode** across the motor/buzzer terminals for protection.
5. **Connect power**: 3.7V LiPo positive to `VCC`, negative to `GND`.
6. **Upload the firmware** (see below), then test each component.

---

## ⬆️ Flashing the Firmware

Use the 6-pin CP2102 module to upload the code:

1. Connect programmer to Pro Mini:
   * `DTR` → `DTR`
   * `RX` → `TX`
   * `TX` → `RX`
   * `VCC` → `VCC`
   * `GND` → `GND`
2. Open `src/RoadSentinel.ino` in the Arduino IDE.
3. Select board: **Arduino Pro or Pro Mini**, choose the correct processor variant (e.g., ATmega328P 3.3V 8MHz or 5V 16MHz) matching your module.
4. Select the COM port of the CP2102 and click **Upload**.
5. Open the **Serial Monitor** at `9600` baud — you should see `RoadSentinel Initialized.`

---

## ⚙️ Adjusting the Sensitivity

* **More sensitive (alarm faster):** decrease `sleepThreshold` in `src/RoadSentinel.ino`, e.g.:
  ```cpp
  const unsigned long sleepThreshold = 1500; // 1.5 seconds
  ```
* **Less sensitive (fewer false alarms):** increase it to `5000` (5 s) — good for systems used with sunglasses or uneven sensor alignment.
* **Role change:** swap the `HIGH`/`LOW` logic if your IR module is active-`LOW` (some modules vary by model). Test with a covering hand over the sensor.

---

## 🧪 Testing the System

1. Power on: serial monitor prints `RoadSentinel Initialized.`
2. Look normally at the sensor → no alarm.
3. Close your eyes (or cover the sensor) for > 2 seconds → **buzzer sounds + motor vibrates** and `ALERT: Drowsiness Detected!` prints over serial.
4. Open your eyes → alarm stops immediately.

---

## 🔍 Troubleshooting

| Symptom | Likely Cause | Fix |
| :--- | :--- | :--- |
| No buzzer/motor at all | Transistor base not driven / bad wiring | Check `D3` → 1kΩ → base path; check GND continuity. |
| Alert never triggers | Sensor polarity is inverted | Swap `HIGH`/`LOW` logic to match your IR module. |
| Alarm false-positives | Sensor misaligned or too far from eye | Reposition sensor closer to the eye; adjust `sleepThreshold`. |
| Upload fails | Wrong board/port or RX/TX swapped | Select correct Pro Mini variant and COM port; cross RX↔TX. |
| Motor causes resets | Back-EMF spikes | Ensure flyback diode is installed with correct polarity. |

---

## 📂 Repository Structure

```
RoadSentinel/
├── README.md                 # This documentation
├── .gitignore                # Ignores Arduino build artifacts
├── hardware/                 # Circuit schematics / images (upload here)
│   └── .keep                 # Placeholder so Git tracks this folder
└── src/
    └── RoadSentinel.ino      # Arduino firmware (non-blocking millis logic)
```

**Circuit Diagram**

![RoadSentinel Circuit Connection](hardware/circuit_diagram.png)
*(Upload your schematic/fritzing diagram to the `hardware/` folder to render this image.)*

---

## ⚠️ Safety Disclaimer

RoadSentinel is a **drowsiness aid, not a substitute for safe driving practices**. It does not guarantee wakefulness and must NEVER be relied upon as the sole safety measure. If you feel tired, **pull over and rest**. Always obey local traffic laws and drive responsibly.

---

## 📜 License

This project is open for educational and personal use. Attribution to the original author is appreciated.