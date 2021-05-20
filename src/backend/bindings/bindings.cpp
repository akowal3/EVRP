//
// Created by akowal3 on 19/05/2021.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <edge.hpp>
#include <router.hpp>
#include <sstream>

namespace py = pybind11;

PYBIND11_MODULE(bindings, m) {
    py::class_<BuildingEdge>(m, "BuildingEdge")
            .def(py::init<unsigned, unsigned, unsigned, unsigned>())
            .def("get_travel_time", &BuildingEdge::get_travel_time)
            .def_readwrite("fromID", &BuildingEdge::from)
            .def_readwrite("toID", &BuildingEdge::to)
            .def_readwrite("distance", &BuildingEdge::distance)
            .def_readwrite("max_speed", &BuildingEdge::max_speed);

    py::class_<Car>(m, "Car")
            .def(py::init<double>());

    py::class_<RouterResult>(m, "RouterResult")
            .def_readwrite("nodes", &RouterResult::nodes)
            .def_readwrite("arcs", &RouterResult::arcs)
            .def_readwrite("socs_in", &RouterResult::socs_in)
            .def_readwrite("socs_out", &RouterResult::socs_out)
            .def_readwrite("charges", &RouterResult::charges)
            .def_readwrite("charge_times", &RouterResult::charge_times)
            .def_readwrite("total_time", &RouterResult::total_time)
            .def_readwrite("charge_time", &RouterResult::charge_time)
            .def("__repr__",
                 [](const RouterResult &r) {
                     std::ostringstream os;
                     os << r;
                     return os.str();
                 });

    py::class_<Router>(m, "Router")
            .def(py::init<unsigned, const std::vector<BuildingEdge> &>())
            .def(py::init<const std::vector<Node> &, const std::vector<BuildingEdge> &>())
            .def("route", &Router::route);

    py::enum_<label_type>(m, "label_type")
            .value("CHARGE_80", label_type::CHARGE_80)
            .value("CHARGE_70", label_type::CHARGE_70)
            .value("CHARGE_MINIMUM", label_type::CHARGE_MINIMUM)
            .value("CHARGE_MAXIMUM", label_type::CHARGE_MAXIMUM)
            .value("NO_CHARGE", label_type::NO_CHARGE)
            .export_values();

    py::enum_<charger_type>(m, "charger_type")
            .value("SLOW_50KW", charger_type::SLOW_50KW)
            .value("FAST_175KW", charger_type::FAST_175KW)
            .export_values();

    py::class_<Node>(m, "Node")
            .def(py::init<unsigned, double, charger_type>())
            .def_property_readonly("ID", &Node::id)
            .def_property_readonly("charge_level", &Node::soc)
            .def_property_readonly("type", &Node::supported_types)
            .def("__repr__",
                 [](const Node &n) {
                     std::ostringstream os;
                     Car c = Car();
                     os << n.id() << " " << n.best_compatible_type(c);
                     return os.str();
                 });

    py::class_<Edge>(m, "Edge")
            .def("get_travel_time", &Edge::get_travel_time)
            .def("tailID", &Edge::tailID)
            .def("headID", &Edge::headID)
            .def("start_charge_level", &Edge::start_charge_level)
            .def("end_charge_level", &Edge::end_charge_level)
            .def_property_readonly("distance", &Edge::get_distance)
            .def_property_readonly("speed", &Edge::get_speed)
            .def_property_readonly("destination", &Edge::destinationCharger)
            .def_property_readonly("source", &Edge::sourceCharger);
}