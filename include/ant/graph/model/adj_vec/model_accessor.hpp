#pragma once


namespace ant::graph::model::adj_vec {

template <typename Model>
struct ModelAccessor {

    Model& model;

    explicit ModelAccessor(Model& model) : model(model) {}

    auto& vertices_info() {
        return model.vertices_info;
    }
};

}