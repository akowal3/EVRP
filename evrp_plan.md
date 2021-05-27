# Introduction

## Chapter descriptions

# Background

## EV modelling and specifics

Explain how EVs are different to standard cars and why 

### Energy consumption and range estimation

- data based model - requires a lot of datapoints
- physics based model - less accurate, doesn't take into account human factors/traffic

Due to suimplicity physics based modelling will be used in this project

### EV charging

Explain how charging power is affected by the SoC of the EV -> it's more beneficial to stay in the 20-80% range as it saves battery wear&tear as well as saves time.

## Graph and routing theory

Explain in detail the underlying theory behind algorithm

Establish notation for representing graphs, edges, cars and constraints

### Dijkstra's algorithm

Desribe the most common and widely used routing algorithm
Describe drawbacks, describe how it isn't a great fit to use for ev routing

### Bellman-Ford algorithm

Describe how it can deal with negative edges, but has larger complexity than Dijkstra

### Bicriteria Shortest Path Algorithm

This is the basis of the algorithm used in this project. It is a modification of Dijsktra, which allows to set the values as we go.

## Previous work
Analysis of articles/papers/theses - very similar to the one in the interim report.

## Existing Solutions

### Routing engines

- Open Source Routing Machine (OSRM)
- Valhalla
- Graphhopper
- Mapbox

### Routing libraries

- Customisable Route Planning (CRP)
- RouitngKit

### EV routing solutions

- TomTom Long Distance EV Routing
- EVTripPlanner
- A Better Routeplanner (ABRP)
- Built-in navigation

# Project Specification

## Project contribution

## User survey analysis

## Project requirements

### Functional requirements

### Non-functional requirements

## Design decisions

- use simplified graph between chargers instead of a full graph in .osm format. The assumption is that in long distance travel, the path will still follow the main roads instead of the local ones, so using a standard car routing should be enough. For long distances the time necessary to drive on the local roads outweigths the benefit of consuming less energy and charging less frequently. Moreover, such approach ensures that there energy consumption will always be positive along the edge (recupreration by regenerative breaking is outweight by the energy required to drive longer distances)
- used C++ as basis for the project
- tried to be as extendable as possible
- keep routing request to a single object so that all the information is always available to the algorithm
- create a graph between the chargers to speed up the processing
- use physics based consumption model model because it's easily computable and can be improved in the future
- have the ability to model different chargers - both speed and plugs
- focus on the travel time as it was the dominant request in the survey, but keep options available
- use OSRM as routing provider because it's one of the most popular routing engines, and can return queries in real time. Moreover, it was easy to set up my own instance of OSRM service, which is free to use, doesn't require any additional API keys and doesn't impose additional constraints on the project.

# Analysis and Design

**High level** overview of the implementation

## Algorithm description

### Graph

Describe how the graph is looks like: what information is stored in the `Edge`, `Node` and `Car` and how they interoperate

### RoutingKit backend description

Routing kit is slower to set up, requres more memory and is not as versitile. The biggest downside is the fact that to model different charge levels, the backend duplicate the nodes ie. for having 100% charge, 80% charge and 70% charge, instance of each node is created 2 additional times. This, combined with edge multiplication to model different speeds, means that the graph grows as `n**3 * m**2` where `n` is number of charge states and `m` is number of speed edges.

### Bicriteria Shortest Path backend description

BSP algorithm does not multiplicate the nodes therefore it only grows as `m**2`, where `m` is number of different-speed edges. It keeps the state between queries, therefore does not have to be rebuilt whenever new query comes in.

Modification of Router can be done in runtime, which allows for insertion of source and destination nodes, which are not chargers.

Explain what is a `Label` - a way of getting to a certain node (contains edge ie. speed and distance), but also stores information about the path taken to get to this point.

## Backend server initializtion and graph building

Explain how OSRM is used to get distance/time calculations for a connection between any 2 chargers.

## Example query walkthrough

Description of a sample query and the dataflow throug the EVRP.

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
1. Response path is passed to OSRM, which provdes path with waypoints (used to display)
1. Reponse path is passed to OSM to obtain information about the chosen chargers (used to diplay)
1. All gathered information is converted into a JSON file and sent as a response to the frontend

# Implementation details

## Algorithm

C++ was used to implement the algorithm due to it's strong static typing, fast execution times

C++ was also used because I wanted to use RoutingKit library, which is only available for C++ platform. At the end I decided to go with other approach, but C++ object orientataion was increadibly helpful through the development process. I tried to follow responsibility based design, where each object is responsible for its data and its processing. This allows for an easy addition of new faetures or improvement to the exisitng ones.

## Backend

Used flask as web-framework
Very simple - processes single post request, which contains a JSON file with all the requrements for the route given by the user.
Call C++ implementation of the algoritm, build response JSON and send it back.

## Frontend

Used angular as frontend framework, leaflet for displaying map

Very simple implementation, for demonstation purposes. The goal was to be able to visually display the proposed the path, which would allow for better comparison against other services such as ABRP and EVTripPlanner.

# Testing

## Test paths

Longer paths, which show the ability of the algorithm to choose to drive slower, charge only to necessary values etc.

- compare against ABRP
- compare against EVTripPlanner
- compare against path produced manually

This will be a mostly empirical comparison, as there is no easy way to quantify the differences in the proposed paths. Travel/charge times and energy consumption can be used to quantify the differences, however, I anticipate that such comparison would be skewed due to the inaccuracies in the models used by my project.

Compare the paths taken by Renault Zoe and Tesla Model 3 - 2 most popular cars in the EV user survey.

## Unit testing

Developed unit tests for the graph to aid the addition of new features, easily test standalone features and methods and finally test the whole pipeline.

Wrote test structs where possible to be able to quicly add new tests without additional boilerplate code.

Unit tests split between different components of the project, which ensured that I didn't break things as I added new faetures/changed existing ones.

## Backend/Frontend testing

Because the focus on the project was on the development of algorithm, the backned and frontend services were not tested thoroughly with use of unit tests. They are also not very reliable and error resilient, but that was not important for the project. They are used as a visualisaton tool mostly.

# Evaluation

## Changes from Interim Report

- focus on travel time and provide the ability to add different route constraints in the future
- CI service was not used to automatically run unit tests, but it appeared as an unnecessary distraction from the project.
- Databases for storing car information were not used, however the backend service can be easily extended to allow for larger EV support. In this project 2 cars were used to demonstate ability to change the car.
- RoutingKit is no longer the basis of the algorithm, but this option was explored in the process.

## Things I'd have done differently

- Not rely on the existing library, as it constraints the development process
- Use Go, as it has better testing, comparable execution time and I could have written backend server is go instead of combining C++ and Python
- Don't focus on the app, as it is not the underlying part of the project
- Start working earlier

# Future work

The project has been developed in a way, which enables for an easy updates in the future.

## Algorithm improvements

- use height information
- prune charger graph more aggresively
- precalculate energy optimal speed to traverse an edge between 2 chargers
