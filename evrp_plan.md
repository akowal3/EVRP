# Title page

# Abstract

# Table of contents

# Introduction

Mostly copied from the interim report, but with more focus on what is the aim of the report and how it has been achieved.

The project aims to research existing solutions, choose the algorithm best suited for the EVRP task and create a prototype app around it.

## Chapter descriptions

# Background

## Previous work

Analysis of articles/papers/theses - copy the one in the interim report.

## Existing Solutions

Overview of existing solutions and how they can be used to create a different approach
Extended/Copied in current form from the interim report.

### Routing engines

- Open Source Routing Machine (OSRM)
- Valhalla
- Graphhopper
- Mapbox

### Routing libraries

- Customisable Route Planning (CRP)
- RouitngKit

### EV routing solutions

- TomTom Long Distance EV Routing: they didn't reply to my emails asking to give me a test API key to try it out...
- EVTripPlanner
- A Better Routeplanner (ABRP)
- Built-in navigation

## EV modelling

Explain how EVs are different from conventional combustion engine vehicles and why standard routing does not work for EVs

### Energy consumption and range estimation

- data-based model - requires a lot of data points
- physics-based model - less accurate, does not take into account human factors/traffic

Due to the simplicity and easy expandability in the future, physics-based modelling is used in this project.

### EV charging

Explain how charging power is affected by the SoC of the EV -> it is more beneficial to stay in the 20-80% range as it saves battery wear&tear as well as saves time.

## Graph and routing theory

In this section, I will explain the underlying theory behind different routing algorithms. I'll talk about whether they can be used in the EVRP setting and show potential issues with them.

I will also establish mathematical notation for representing graphs, edges, cars and constraints.

### Dijkstra's algorithm

It is the most basic algorithm to solve the graph traversal, and consequently shortest path routing problem efficiently.
Describe drawbacks, describe why it isn't a great fit to use for EVRP.

### Bellman-Ford algorithm

Describe how it can deal with negative edges, but has larger complexity than Dijkstra. Explain how negative cycles in the graph can be avoided.

### Bicriteria Shortest Path Algorithm

This is the basis of the algorithm used in this project. It is a modification of Dijkstra, which allows us to set the 'weights' as we go and consider multi-criteria weighting as long as the label-setting requirement is met and the dominance criterion is defined. In the case of EVRP, the label ie. a way of getting to node `n` dominates another label if the total journey time (including charging) was less than L2's journey time and the SoC is no worse than L2's.

# Project Specification

## Project contribution

## User survey analysis

## Project requirements

The main goal of the project was to develop an algorithm:

- allow to model different chargers and their underlying characteristics and compatibility.
- have an option to reduce the driving speed to save energy and allow to drive longer distances.

### Functional requirements

### Non-functional requirements

## Design decisions

- use a simplified graph between chargers instead of a full graph in .osm format. The assumption is that in long-distance travel, the path will still follow the main roads instead of the local ones, so using a standard car routing should be enough. For long distances, the time necessary to drive on the local roads outweighs the benefit of consuming less energy and charging less frequently. Moreover, such an approach ensures that their energy consumption will always be positive along the edge (recuperation by regenerative braking is outweighed by the energy required to drive long distances)
- used C++ as the basis for the project
- tried to be as extendable as possible
- keep routing request to a single object so that all the information is always available to the algorithm
- create a graph between the chargers to speed up the processing
- use physics-based consumption model because it is easily computable and can be improved in the future
- have the ability to model different chargers - both speed and plugs
- focus on the travel time as it was the dominant request in the survey, but keep options available
- use OSRM as a routing provider because it's one of the most popular routing engines, and can return queries in real-time. Moreover, it was easy to set up a private instance of the OSRM service backend, which is free to use, doesn't require any additional API keys and doesn't impose additional constraints on the project.

# Analysis and Design

**High level** overview of the implementation and explanation of internal workings of the algorithm. Description of work done, which is not included in the final product ie. RoutingKit backend.

## Algorithm description

### Graph

Describe how the graph looks like: what information is stored in the `Edge`, `Node` and `Car` and how they interoperate

### RoutingKit backend description

RoutingKit is slower to set up, requires more memory and is not as versatile. The biggest downside is the fact that to model different charge levels, the backend duplicates the nodes ie. for having 100% charge, 80% charge and 70% charge, an instance of each node is created 2 additional times. This, combined with edge multiplication to model different speeds, means that the graph grows as `n**3 * m**2` where `n` is the number of charge states and `m` is the number of speed edges.

### Bicriteria Shortest Path backend description

**This is the most important section of the report as it directly explains how the algorithm works.**

BSP algorithm does not multiplicate the nodes therefore it only grows as `m**2`, where `m` is the number of different-speed edges. It keeps the state between queries, therefore does not have to be rebuilt whenever a new query comes in.

Modification of Router can be done in runtime, which allows for insertion of source and destination nodes, which are not chargers.

Explain what is a `Label` - a way of getting to a certain node (contains edge ie. speed and distance), but also stores information about the path taken to get to this point.

Explain the domination criteria, discuss other possible domination options

## Backend server initialization and graph building

Explain how OSRM is used to get distance/time calculations for a connection between any 2 chargers.

## Example query walkthrough

Description of a sample query and the dataflow through the EVRP.

Create a diagram to better illustrate the architecture and processing pipeline.

Frontend:

1. user chooses the route requirements using UI
1. requirements are converted into JSON document and sent to the backend server

Backend:

1. JSON request is converted into a Car object, which holds the information about the query.
1. Start and finish points are temporarily added to the graph to allow for first/last mile routing
    - OSRM is queried to find the connections between start/endpoints and the nodes in the graph
    - In that way, the graph is updated on the fly with only necessary connections

1. Algorithm is queried to find a path from start to end through the chargers
1. Response path is passed to OSRM, which provides a path with waypoints (used to display)
1. Response path is passed to OSM to obtain information about the chosen chargers (used to display)
1. All gathered information is converted into a JSON file and sent as a response to the frontend

# Implementation details

## Algorithm

C++ was used to implement the algorithm due to its strong static typing, fast execution times

C++ was also used because I wanted to use the RoutingKit library, which is only available for the C++ platform. In the end, I decided to go with another approach, but C++ object orientation was incredibly helpful through the development process. I tried to follow responsibility based design, where each object is responsible for its data and its processing. This allows for the easy addition of new features or improvement to the existing ones.

Show how objects interact with each other

## Backend

Used flask as web-framework
Very simple - processes single post request, which contains a JSON file with all the requirements for the route given by the user.
Call C++ implementation of the algorithm, build response JSON and send it back.

## Frontend

Used angular as frontend framework, leaflet for displaying map

Very simple implementation, for demonstration purposes. The goal was to be able to visually display the proposed path, which would allow for better comparison against other services such as ABRP and EVTripPlanner.

# Testing

## Test paths

Longer paths, which show the ability of the algorithm to choose to drive slower, charge only to necessary values etc.

- compare against ABRP
- compare against EVTripPlanner
- compare against path produced manually

This will be a mostly empirical comparison, as there is no easy way to quantify the differences in the proposed paths. Travel/charge times and energy consumption can be used to quantify the differences, however, I anticipate that such comparison would be skewed due to the inaccuracies in the models used by my project.

Compare the paths taken by Renault Zoe and Tesla Model 3 (the 2 most popular cars in the EV user survey).

## Unit testing

Developed unit tests for the graph to aid the addition of new features, easily test standalone features and methods and finally test the whole pipeline.

Wrote test structs where possible to be able to quickly add new tests without additional boilerplate code.

Unit tests split between different components of the project, which ensured that I didn't break things as I added new features/changed existing ones.

## Backend/Frontend testing

Because the focus of the project was on the development of the algorithm, the backend and frontend services were not tested thoroughly with aid of unit tests. They are also not very reliable and error-resilient, but that was not important for the project. They are used as a visualisation tool mostly.

# Evaluation

## Changes from Interim Report

- focus on travel time and provide the ability to add different route constraints in the future
- CI service was not used to automatically run unit tests, but it appeared as an unnecessary distraction from the project.
- Databases for storing car information were not used, however, the backend service can be easily extended to allow for larger EV support. In this project, 2 cars were used to demonstrate the ability to change the car.
- RoutingKit is no longer the basis of the algorithm, but this option was explored in the process.

## Things I'd have done differently (with different tile)

- Not rely on the existing library, as it constraints the development process. I've spent a lot of time trying to get it to work only to abandon it in the end for a better alternative. This approach has potential, but requires more advanced energy consumption estimation and is better suited for the consideration of full-map-graphs instead of simplified charger-graphs.
- Use Go, as it has better testing, comparable execution time and I could have written backend server in Go instead of combining C++ and Python
- Don't focus on the app, as it is not the underlying part of the project
- Start working earlier

# Future work

The project has been developed in a way, which enables easy updates in the future.

## Algorithm improvements

- use height information
- prune charger graph more aggressively to reduce its memory footprint and reduce the number of edges required to be processed by the algorithm
- precalculate energy optimal speed to traverse an edge between 2 chargers
- use better energy consumption estimation, which would take into account characteristics of the road, traffic and weather conditions
- add the charging cost to the consideration. One user suggested a slider to choose a tradeoff between cost and travel time, which could be interesting to explore further. My approach would be to have the slider set weights for travel time and cost, and when assessing domination of labels create a linear combination of normalized `travel time` * `weight` and `cost` * `weight` and compare it (along with previously mentioned not worse SoC of the battery) to determine the label dominance.

## App improvements

Mention the fact that frontend and backend were created for demonstration purposes, so they could use some polish - both visual and internal. Most importantly error handling should be done better to ensure a smooth user experience. Moreover, the way I add temporary nodes to the graph could also be improved. I could cache the queries from OSRM instead of calculating distances/times every time a new EVRP query comes.

A car database could be created to offload the car specific information and ensure its correctness (potentially sync with ev-database.org).

Do map in unity to look better

# Bibliography

# User Guide