/*
Copyright 2016 Soramitsu Co., Ltd.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 */

#ifndef NETWORK_GRPC_CALL_HPP
#define NETWORK_GRPC_CALL_HPP

#include <grpc++/grpc++.h>
#include <assert.h>

namespace network {

  /*
   * TODO(motxx): Use this and clarify ownership.
  class ReferenceCountable {
  public:
    void ref() {
      count_++;
    }

    void unref() {
      assert(count > 0);
      count_--;
      if (!count_) {
        delete this;
      }
    }

  private:
    size_t count_ = 0;
  };
  */

  /**
   * to enable various Call instances to process in ServiceHandler::handleRpcs() by polymorphism.
   * @tparam ServiceHandler
   */
  template <typename ServiceHandler>
  class UntypedCall {
  public:
    UntypedCall(State const& state)
      : state_(state) {}

    virtual ~UntypedCall() {}

    /**
     * invokes when state is RequestReceivedTag.
     * @param serviceHandler - an instance that has all rpc handlers. e.g. CommandService
     */
    virtual void requestReceived(ServiceHandler& serviceHandler) = 0;

    /**
     * invokes when state is ResponseSentTag.
     */
    virtual void responseSent() = 0;

    /**
     * selects a procedure by state and invokes it by using polymorphism.
     * this is called from ServiceHandler::handleRpcs()
     * @param serviceHandler - an instance that has all rpc handlers. e.g. CommandService
     */
    void onCompleted(ServiceHandler& serviceHandler) {
      switch (callback_) {
        case State::RequestCreated: {
          call_->requestReceived(serviceHandler);
          break;
        }
        case State::ResponseSent: {
          call_->responseSent();
          break;
        }
      }
    }

    enum class State { RequestCreated, ResponseSent };

  private:
    const State state_;
  };

  /**
   * to manage the state of one rpc.
   * @tparam ServiceHandler
   * @tparam AsyncService
   * @tparam RequestType
   * @tparam ResponseType
   */
  template <typename ServiceHandler, typename AsyncService, typename RequestType, typename ResponseType>
  class Call : public UntypedCall<ServiceHandler> {
  public:
    Call(RpcHandler const& rpcHandler)
      : rpcHandler_(rpcHandler) {}

    virtual ~Call() {}

    /**
     * invokes when state is RequestReceivedTag.
     * this method is called by onCompleted() in super class (UntypedCall).
     * @param serviceHandler - an instance that has all rpc handlers. e.g. CommandService
     */
    void requestReceived(ServiceHandler* serviceHandler) override {
      rpcHandler(*this);
    }

    /**
     * invokes when state is ResponseSentTag.
     * this method is called by onCompleted() in super class (UntypedCall).
     * @param serviceHandler - an instance that has all rpc handlers. e.g. CommandService
     */
    void responseSent() override {
    }

    /**
     * notifies response and grpc::Status when finishing handling rpc.
     * @param status
     */
    void sendResponse(const ::grpc::Status& status) {
      responder_.Finish(response, status, &ResponseSentTag);
    }

    /**
     *
     * @param serviceHandler
     * @param cq
     * @param requestMethod
     * @param rpcHandler
     */
    static void enqueueRequest(AsyncService* asyncService,
                               ::grpc::ServerCompletionQueue* cq,
                               RequestMethod requestMethod,
                               RpcHandler rpcHandler) {
      auto call = new Call<ServiceHandler, AsyncService, RequestType, ResponseType>(rpcHandler);

      (asyncService->*requestMethod)(&call->ctx_, &call->request(),
                                     &call->responder_, cq, cq,
                                     &call->RequestReceivedTag);
    }

  public:
    auto& request()  { return request_; }
    auto& response() { return response_; }

  private:
    const UntypedCall<ServiceHandler> RequestReceivedTag { State::RequestCreated };
    const UntypedCall<ServiceHandler> ResponseSentTag { State::ResponseSent };

  private:
    const RpcHandler rpcHandler_;
    RequestType request_;
    ResponseType response_;
    ::grpc::ServerContext ctx_;
    ::grpc::ServerAsyncResponseWriter<ResponseType> responder_;
  };

}  // namespace network

#endif  // NETWORK_GRPC_CALL_HPP
