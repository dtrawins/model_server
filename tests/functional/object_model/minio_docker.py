#
# Copyright (c) 2020 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import config
from object_model.docker import Docker
from utils.model_management import minio_condition


class MinioDocker(Docker):

    def __init__(self, request, container_name, start_container_command=config.start_minio_container_command,
                 env_vars_container=None, network="", image=config.minio_image):
        super().__init__(request, container_name, start_container_command,
                         env_vars_container, network, image)
        self.start_container_command = start_container_command.format(self.grpc_port)

    def start(self):
        self.start_container_command = self.start_container_command.format(self.grpc_port)
        return super().start()

    def wait_endpoint_setup(self, condition=minio_condition, timeout=60,
                            container_log_line=config.container_minio_log_line):
        return super().wait_endpoint_setup(condition, timeout, container_log_line)