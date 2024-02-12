#include <filesystem>
#include <json.hpp>
#include "opensplat.hpp"
#include "point_io.hpp"
#include "utils.hpp"
#include "cv_utils.hpp"

namespace fs = std::filesystem;
using namespace torch::indexing;

int main(int argc, char *argv[]){
    std::string projectRoot = "banana";
    const float downScaleFactor = 2.0f;
    const int numIters = 1000;
    const int numDownscales = 3;
    const int resolutionSchedule = 250;
    const int shDegree = 3;
    const int shDegreeInterval = 1000;
    const float ssimLambda = 0.2f;

    torch::Device device = torch::kCPU;

    if (torch::cuda::is_available()) {
        std::cout << "Using CUDA" << std::endl;
        device = torch::kCUDA;
    }else{
        std::cout << "Using CPU" << std::endl;
    }

    ns::InputData inputData = ns::inputDataFromNerfStudio(projectRoot);
    
    ns::Model model(inputData.points, 
                    numDownscales, resolutionSchedule, shDegree, shDegreeInterval,
                    device);
    model.to(device);

    // TODO: uncomment
    // for (ns::Camera &cam : inputData.cameras){
    //     cam.loadImage(downScaleFactor);
    // }

    InfiniteRandomIterator<ns::Camera> cams(inputData.cameras);

    for (size_t step = 0; step < numIters; step++){
        // ns::Camera cam = cams.next();
        ns::Camera cam = inputData.cameras[6];
        
        // TODO: remove
        cam.loadImage(downScaleFactor);

        model.optimizersZeroGrad();

        torch::Tensor rgb = model.forward(cam, step);
        torch::Tensor gt = cam.getImage(model.getDownscaleFactor(step));
        gt = gt.to(device);

        torch::Tensor ssimLoss = 1.0f - model.ssim.eval(rgb, gt);
        torch::Tensor l1Loss = ns::l1(rgb, gt);
        torch::Tensor mainLoss = (1.0f - ssimLambda) * l1Loss + ssimLambda * ssimLoss;
        mainLoss.backward();

        model.optimizersStep();
        std::cout << model.means.grad();
        exit(1);
        //model.optimizersScheduleStep(); // TODO

    }
    // inputData.cameras[0].loadImage(downScaleFactor);  
    
}