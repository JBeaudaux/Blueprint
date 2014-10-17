Blue print watch
================

The Blue print open smart-watch is designed to detect sudden health onsets, thus enabling preventive measures to be taken or notifying caretakers for improved and quicker emergency response. Precisely, the role of the Blue print smart-watch is threefold:
- Early detection of heart failure: Combined, factors such as ambient temperature, physical activity and nutrition have a strong impact on one's health, and may quickly be the cause of heart failure. Those can often be avoided by simply resting a few mements before the stroke actually occur. In this context, it is important to assess risk-level in real-time, in order to notify the user and allow him to rest in time to prevent imminent strokes. Through the constant monitoring and analysis of vital parameters, the Blue print smart-watch can detect at-risk situation before they actually occur, and notify the person and its caretakers when taking action may save his life.
- Alarm button: The watch is also equipped with an alarm button. When pressed, an emergency message is sent to a base station, that immediately sends an SMS/email notifying relatives or care services, thus allowing for quicker emergency management.
- Fall detection: Many elders have experienced falls, without being able to get back on their feet and call for help. The watch thus integrates a movement patters monitoring and analysis component, able to quickly detect critical falls and immediatly notify relatives or care services (similarly to the alarm button).


General notice
--------------

The source code for the Blue print smart-watch is divided in two parts:
- The firmware of the watch, that can be deployed on ez430 chronos watch. It enables fall detection, heart attack prevention and an emergency alarm system. 
- The control station framework, that can run on UNIX systems. It includes a control mode, that detects the watch emergency alarm system messages and notify caretakers when triggered, as well as a demo mode that displays the watch sensed data (e.g. movement, heart rate) for public advertisement of the watch capabilities.


Requirements
------------

The watch firmware is intended to be compiled on Linux environment using msp430-gcc version 4.6.3, and to be flashed to the watch through mspdebug. The control center requires python version 2.7.6. Some additional tools provided also require Gnuplot 4.6. The stresstesting of the provided firmware and control center has been tested accordingly.