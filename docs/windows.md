# Window Reservation

**NOTE:** Deciphering this document is not part of the game or a challenge. If you find any of this unclear please ask admin

## What is a contact

A contact is the duration of time where a ground station is in communication with the ground. During this time game admin will have contact with the satellite.

During a contact admin will.
- Upload any user scripts
- Upload user commands 
- Download tlemetry
- Download extra challenge related data
- Download images

## What is a blackout

A blackout occurs during the time between contacts. Basically a blackout is any period of time when the satellite is not in contact with the ground. Teams may operate the satellite only during blackouts.

## What is a window

Each blackout is split into 5 time windows. Teams may reserve one window each per blackout. This allows every team to have a chance to execute commands and scripts on the satellite during each blackout. Windows are equal in length. 

A period of time before the end of each blackout is reserved for administrative action and preparing for contact.

A small time period at the beginning of each window is used to reset the apps on the satellite. This means that there is risk in conducting operations at the very beginning and very end of your window. If they do not complete in time they may be cut off.

## Reserving a window

The user api can be used to reserve one window per blackout for each team. 

Window reservation occurs on a first come first serve basis.

Remember that the satellite will be in a different position in its orbit during the different windows. Which window you pick may be very important to what you are trying to do.

## Dropping a reservation

If you decide you do not like the window you have reserved you can use the api to "unreserve" your window.

## Adding tasks to a window

The tasking api describes multiple tasks that you can run on the satellite.

None of the challenges in the game involve "Denial of Service" to any of the satellites devices or the satellite itself. Please do not spam the satellite with pointless tasks.

## Clearing tasks from a window

The api has a command which will clear your current reserved window of all tasks.

Unreserving a window clears all tasks from that window.

There is no command do remove an individual task from a window.

## When can I reserve a window in a specific blackout

The tasking api will accept window reservations and task requests during blackout periods. However, the blackout period for which you are reserving a window/requesting tasks is the **NEXT** blackout period. An example contact/blackout schedule is below

| Start Time | End Time | Contact/Blackout | What is the satellite doing |
| ---------- |--------- | ---------------- | ---------------------------- |
| 2023-07-11 12:00:00 | 2023-07:11 12:10:00  |  Blackout #0   | -  |
| 2023-07-11 12:00:00 | 2023-07:11 12:10:00  |  Contact #1   |  Uplinking tasks/scripts to be executed in blackout #1 \n dowlinking tlm from blackout #0  |
| 2023-07-11 12:10:00 | 2023-07:11 13:00:00  |  Blackout #1  |  Running blackout #1 task load  |
| 2023-07-11 13:00:00 | 2023-07:11 13:10:00  |  Contact #2   |  Uplinking tasks/scripts to be executed in blackout #2 \n dowlinking tlm from blackout #1  |
| 2023-07-11 13:10:00 | 2023-07:11 13:00:00  |  Blackout #2  |  Running blackout #2 task load |
| 2023-07-11 14:00:00 | 2023-07:11 14:10:00  |  Contact #3   |  Uplinking tasks/scripts to be executed in blackout #3 \n dowlinking tlm from blackout #2  |
| 2023-07-11 14:10:00 | 2023-07:11 14:00:00  |  Blackout #3  |  Running blackout #3 task load |
| 2023-07-11 15:00:00 | 2023-07:11 15:10:00  |  Contact #4   |  Uplinking tasks/scripts to be executed in blackout #4 \n dowlinking tlm from blackout #3  |
| 2023-07-11 15:10:00 | 2023-07:11 15:00:00  |  Blackout #4  |  Running blackout #4 task load  |

For example:

This means that during **blackout #2** teams will be
- Analyzing data generated during **blackout #1**
- Reserving a window and generating tasks for **blackout #3**

