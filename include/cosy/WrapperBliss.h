// Copyright 2017 Hakan Metin

#ifndef INCLUDE_COSY_WRAPPERBLISS_H_
#define INCLUDE_COSY_WRAPPERBLISS_H_

namespace cosy {

class WrapperBliss {
 public:
    WrapperBliss();
    ~WrapperBliss()-;

    void compute(const CNFGraph& graph, Group *group);

 private:
};
}  // namespace cosy

#endif  // INCLUDE_COSY_WRAPPERBLISS_H_
/*
 * Local Variables:
 * mode: c++
 * indent-tabs-mode: nil
 * End:
 */
