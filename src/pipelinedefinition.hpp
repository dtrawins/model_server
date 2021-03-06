//*****************************************************************************
// Copyright 2020 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//*****************************************************************************
#pragma once

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#include "tensorflow_serving/apis/prediction_service.grpc.pb.h"
#pragma GCC diagnostic pop

#include "model_version_policy.hpp"
#include "node.hpp"
#include "pipeline.hpp"
#include "status.hpp"

namespace ovms {

class ModelManager;

using pipeline_connections_t = std::unordered_map<std::string, std::unordered_map<std::string, InputPairs>>;

enum class NodeKind {
    ENTRY,
    DL,
    EXIT
};

const std::string DL_NODE_CONFIG_TYPE = "DL model";

Status toNodeKind(const std::string& str, NodeKind& nodeKind);

struct NodeInfo {
    NodeKind kind;
    std::string nodeName;
    std::string modelName;
    std::optional<model_version_t> modelVersion;
    std::unordered_map<std::string, std::string> outputNameAliases;

    NodeInfo(NodeKind kind,
        const std::string& nodeName,
        const std::string& modelName = "",
        std::optional<model_version_t> modelVersion = std::nullopt,
        std::unordered_map<std::string, std::string> outputNameAliases = {}) :
        kind(kind),
        nodeName(nodeName),
        modelName(modelName),
        modelVersion(modelVersion),
        outputNameAliases(outputNameAliases) {}
};

class PipelineDefinition {
    std::string pipelineName;
    std::vector<NodeInfo> nodeInfos;
    pipeline_connections_t connections;
    std::set<std::pair<const std::string, model_version_t>> subscriptions;

private:
    Status validateNode(ModelManager& manager, NodeInfo& node);

public:
    PipelineDefinition(const std::string& pipelineName,
        const std::vector<NodeInfo>& nodeInfos,
        const pipeline_connections_t& connections) :
        pipelineName(pipelineName),
        nodeInfos(nodeInfos),
        connections(connections) {}

    Status create(std::unique_ptr<Pipeline>& pipeline,
        const tensorflow::serving::PredictRequest* request,
        tensorflow::serving::PredictResponse* response,
        ModelManager& manager) const;

    Status validateNodes(ModelManager& manager);
    Status validateForCycles();
    const std::string& getName() const { return pipelineName; }

    void notifyUsedModelChanged() {}

    void makeSubscriptions(ModelManager& manager);
    void resetSubscriptions(ModelManager& manager);

    Status getInputsInfo(tensor_map_t& inputsInfo, ModelManager& manager) const;
};
}  // namespace ovms
