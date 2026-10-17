#include "bvdata.hpp"
#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>

void equal(double a, double b) {
    if (std::abs(a-b)>1e-8) throw std::runtime_error("Numerical mismatch: " + std::to_string(a) + " != " + std::to_string(b));
}
int main() {
    try {
        bvdata::Repository repo;
        repo.runExpression(R"(
            raw <- matrix(1:12, nrow=3)
            cumulative <- raw
            for (x in 1:3) for (y in 1:4) cumulative[x,y] <- sum(raw[1:x,1:y])
            synthetic <- list(p=list(cumulative, cumulative*3), sd=list(cumulative))
            FILES[[1]] <- list(variables=synthetic)
        )");
        bvdata::File f(&repo, "synthetic", 1);
        f.shortname="synthetic"; f.quad=ViewRect(0,0,3,4); f.xcells=3; f.ycells=4; f.num_time_steps=2;
        std::unique_ptr<datatypes::Grid> grid(repo.get2DGrid(&f,"p",1));
        std::unique_ptr<datatypes::Grid> first(repo.get2DGridVariation(&f,"p",1));
        std::unique_ptr<datatypes::Grid> second(repo.get2DGridVariation(&f,"p",2));
        for(int x=0;x<3;++x) for(int y=0;y<4;++y) {
            equal(grid->getValue(x,y), 1+x+3*y);
            equal(first->getValue(x,y), 0);
            equal(second->getValue(x,y), 2*(1+x+3*y));
        }
        for(int x0=0;x0<3;++x0) for(int x1=x0;x1<3;++x1)
        for(int y0=0;y0<4;++y0) for(int y1=y0;y1<4;++y1) {
            double direct=0;
            for(int x=x0;x<=x1;++x) for(int y=y0;y<=y1;++y) direct+=1+x+3*y;
            const auto curve=f.getVariableCurve("p",x0+1,x1+1,y0+1,y1+1);
            equal(curve.at(0),direct); equal(curve.at(1),direct*3);
            equal(f.sumVariableOnSpatialQuadRange("p",ViewRect(x0,y0,x1-x0,y1-y0),0),direct);
        }
        equal(f.sumVariableOnSpatialQuadRange("p",ViewRect(10,10,2,2),0),0);
        bool caught=false;
        try { repo.get2DGrid(&f,"p",0); } catch(const std::exception&) { caught=true; }
        if(!caught) throw std::runtime_error("Invalid time accepted");
        caught=false;
        try { repo.getVariableQuad(&f,"unknown"); } catch(const std::invalid_argument&) { caught=true; }
        if(!caught) throw std::runtime_error("Unknown variable accepted");
        repo.addFilesFromDirectory(BIRDVIS_DATA_DIR);
        if(repo.getNumFiles()!=20) throw std::runtime_error("Expected 20 bundled datasets");
        for (auto *file : repo.files) {
            file->load();
            if(file->getNumTimeSteps()!=52) throw std::runtime_error("Unexpected date count");
            if(file->getDate(0)!=boost::gregorian::date(2009,1,5) || file->getDate(51)!=boost::gregorian::date(2009,12,28))
                throw std::runtime_error("Unexpected date range");
            std::unique_ptr<datatypes::Grid> actual(repo.get2DGrid(file,"p",52));
            double sum=0;
            for(int x=0;x<actual->xcells;++x) for(int y=0;y<actual->ycells;++y) sum+=actual->getValue(x,y);
            double reference=repo.evalDouble(file->shortname+"$p[[52]][250,125]");
            if(std::abs(sum-reference)>1e-5) throw std::runtime_error("Bundled grid integral mismatch");
            std::cout << "Loaded and checked " << file->filename << '\n';
            repo.runExpression("rm("+file->shortname+"); FILES[["+std::to_string(file->id)+"]]<-NA; gc()");
        }
        const auto badPath=repo.evalString("tempfile(fileext='.RData')");
        repo.R["bad_path"]=badPath;
        repo.runExpression("local({unrelated<-42; save(unrelated,file=bad_path)})");
        bvdata::File bad(&repo,badPath,250); bad.shortname="bad_dataset";
        caught=false;
        try { bad.load(); } catch(const std::exception&) { caught=true; }
        repo.runExpression("unlink(bad_path)");
        if(!caught || bad.loaded) throw std::runtime_error("Malformed dataset accepted");
        std::cout << "Grid extraction, all spatial ranges, curves, variation, invalid inputs passed\n";
        return 0;
    } catch(const std::exception &e) { std::cerr << e.what() << '\n'; return 1; }
}
