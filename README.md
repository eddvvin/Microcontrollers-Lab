# Traffic Intersection Controller

A hardware-based traffic signal controller for the intersection of **Glenna Goodacre Blvd and University Ave** in Lubbock, TX. This academic project implements a real-world traffic light sequencing system using an MSP430FR6989 microcontroller, shift register LED expansion, and a custom-designed PCB.

---

## Overview

This project drives **31 traffic signal LEDs** to simulate the signal phases of an actual city intersection. Because the MSP430FR6989 does not have enough GPIO pins to drive 31 LEDs directly, the design uses **four daisy-chained 74HC595 shift registers** to expand output capability while keeping the microcontroller interface to just a handful of pins.

The hardware is being built on a custom PCB designed in KiCad, with all component sourcing done through DigiKey.

### Project Goals

- Replicate the signal phasing of a real Lubbock intersection
- Design a clean, manufacturable PCB using professional layout practices
- Integrate 3.3V logic (MSP430) with 5V peripherals (74HC595) safely
- Provide a robust power solution that can handle the full LED load

---

## Hardware Architecture

### Core Components

| Component | Role |
|-----------|------|
| MSP430FR6989 | Main microcontroller, provides signal timing logic |
| 74HC595 (×4) | Daisy-chained shift registers for LED expansion |
| LM2596 Buck Converter Module | External 12V → 5V power regulation |
| 31× LEDs | Red / Yellow / Green signal indicators |
| SMD 0805 Resistors | Per-LED current limiting |
| Screw Terminal (ED2635-ND) | Secure power input connector |

### System Diagram

```
 12V Input
    │
    ▼
┌──────────────────────┐
│ LM2596 Buck Module   │  (external, 12V → 5V)
└──────────┬───────────┘
           │ 5V
           ├────────────────┐
           │                │
           ▼                ▼
    ┌────────────┐   ┌──────────────┐
    │ MSP430     │──▶│ 74HC595 × 4  │──▶ 31 LEDs (via 360Ω resistors)
    │ (3.3V I/O) │   │ (daisy chain)│
    └────────────┘   └──────────────┘
      [Optional level shifters between MSP430 and shift registers]
```

---

## Key Design Decisions

### Current Limiting Resistors

Settled on **360 Ω SMD 0805 (hand-solder footprint)** for all LEDs, with **470 Ω flagged as a safer alternative**. This decision is driven by the 74HC595's **~70 mA total output current limit per chip** — a critical constraint discovered mid-project. With up to 8 LEDs per shift register, the per-LED current budget must stay well under this ceiling.

- DigiKey PN (360 Ω): `311-360CRCT-ND`
- DigiKey PN (470 Ω): `311-470CRCT-ND`

### PCB Trace Widths

Trace widths were chosen based on current carrying requirements:

- **1.0 mm** — VCC distribution (main power rails)
- **0.5 mm** — Decoupling capacitor connections
- **0.2 mm** — Individual LED signal traces

### Grounding Strategy

A **single continuous ground plane on the bottom copper layer (B.Cu)** handles all ground returns. No separate ground traces are routed on the top layer (F.Cu). This is the standard professional approach and was validated during design review.

### Power Architecture

Originally considered using an onboard **LDO regulator**, but stepping down from 12 V generated excessive heat at the required current. Switched to an **external LM2596-based buck converter module** (Amazon-sourced) per tutor recommendation. This keeps heat off the main board and provides a clean 5 V rail.

### Logic Level Shifting

The MSP430FR6989 operates at **3.3 V logic**, while the 74HC595 shift registers run at **5 V**. To handle this cleanly without committing to one approach up front, the PCB includes **solder jumpers** that allow optional bypass of level shifter ICs. This lets the design be tested both ways on a populated board.

### Connectors

- **Power input:** Screw terminal `ED2635-ND` — chosen over pin headers for secure demo connections
- **Signal headers:** 40-pin breakaway headers `S1011EC-40-ND`, cut to length as needed

---

## Bill of Materials (Summary)

| Part | DigiKey PN | Notes |
|------|-----------|-------|
| 360 Ω resistor, SMD 0805 | 311-360CRCT-ND | Primary choice |
| 470 Ω resistor, SMD 0805 | 311-470CRCT-ND | Safer alternative |
| 40-pin breakaway header | S1011EC-40-ND | Cut to length |
| Screw terminal power connector | ED2635-ND | Power input |
| MSP430FR6989 | (TI) | Main MCU |
| 74HC595 | (×4) | Shift registers |
| LM2596 buck module | Amazon | External 12V→5V |

---

## Project Status

### Completed

- [x] Schematic design in KiCad
- [x] Component selection and current budgeting
- [x] PCB trace width strategy finalized
- [x] Ground plane layout validated
- [x] Power architecture decision (external buck module)
- [x] Connector selection

### In Progress

- [ ] Finalize resistor value (360 Ω vs. 470 Ω) based on per-shift-register current budget
- [ ] Complete DigiKey order (consolidate screw terminal into existing cart)
- [ ] Resolve logic level shifting approach (with or without dedicated level shifter ICs)
- [ ] PCB fabrication and assembly
- [ ] Firmware development and bring-up

---

## Tools Used

- **KiCad** — Schematic capture, PCB layout, and 3D rendering
- **DigiKey** — Component sourcing
- **Code Composer Studio / MSP430 toolchain** — Firmware development (planned)

---

## Team

Two-student academic project.

---

## License

Academic project — license to be determined.
