#include "sci_arma.h"


//fmin with linear constraints
//exterior-point
x_fval sci_arma::fmincon(const obj_fun &f, vec &x0, mat &A, mat &b, const options& opt)
{
    try{
        if((int)x0.n_cols!=1)
            throw std::logic_error("ERROR: fmincon(): x0 must be a column vector.");
    }
    catch(std::exception& e){
        std::cerr<<e.what()<<std::endl;
        std::terminate();
    }
    umat c=A*x0-b < zeros(A.n_rows,1);
    int flag=0;
    for(auto i=0;i<A.n_rows;i++)
    {
        if(c(i,0)==0)
            flag++;
    }
    try{
        if(flag>0)
            throw std::logic_error("ERROR: fmincon(): x0 does not meet linear constraints, or on the boundary.");
    }
    catch(std::exception& e){
        std::cerr<<e.what()<<std::endl;
        std::terminate();
    }
    double delta=1, C=1.1, eps=0.00001;
    long long ite=0;
    auto x_=x0;
    auto P=[&](vec& x)->double {
        vec x1=b-A*x;
        auto n_ineq=x1.n_rows;
        double B=0;
        for(auto i=0;i<n_ineq;i++)
        {
            double temp=std::max(0.0,-x1(i));
            B+=temp*temp;
        }
        return delta*B;
    };
    while (true)
    {
        ite++;
        auto g=[&](vec& x)-> double {
            return f(x) + P(x);
        };
        x_fval result_;
        if(opt.algo == Powell)
        { result_= powell(g, x_);}
        else if(opt.algo == Rosenbrock)
        { result_= rosenbrock(g, x_);}
        x_=result_.x;
        if(P(x_) < eps || ite>=opt.max_ite)
        {
            x_fval result;
            result.x=x_;
            result.fval=f(x_);
            return result;
        }
        else
        {
            delta*=C;
        }
    }
}


x_fval sci_arma::fmincon(const obj_fun &f, vec &x0, mat &A, mat &b, mat& Aeq, mat& beq, const options& opt)
{
    try{
        if((int)x0.n_cols!=1)
            throw std::logic_error("ERROR: fmincon(): x0 must be a column vector.");
    }
    catch(std::exception& e){
        std::cerr<<e.what()<<std::endl;
        std::terminate();
    }
    umat c=A*x0-b < zeros(A.n_rows,1);
    int flag=0;
    for(auto i=0;i<A.n_rows;i++)
    {
        if(c(i,0)==0)
            flag++;
    }
    try{
        if(flag>0)
            throw std::logic_error("ERROR: fmincon(): x0 does not meet linear constraints, or on the boundary.");
    }
    catch(std::exception& e){
        std::cerr<<e.what()<<std::endl;
        std::terminate();
    }
    c=Aeq*x0-beq == zeros(Aeq.n_rows,1);
    flag=0;
    for(auto i=0;i<Aeq.n_rows;i++)
    {
        if(c(i,0)==0)
            flag++;
    }
    try{
        if(flag>0)
            throw std::logic_error("ERROR: fmincon(): x0 does not meet equality constraints.");
    }
    catch(std::exception& e){
        std::cerr<<e.what()<<std::endl;
        std::terminate();
    }
    double delta=1, C=1.1, eps=0.00001;
    long long ite=0;
    auto x_=x0;
    auto P=[&](vec& x)->double {
        vec x1=b-A*x, x2=beq-Aeq*x;
        auto n_ineq=x1.n_rows;
        auto n_eq=x2.n_rows;
        double B=0;
        for(auto i=0;i<n_ineq;i++)
        {
            double temp=std::max(0.0,-x1(i));
            B+=temp*temp;
        }
        for(auto i=0;i<n_eq;i++)
        {
            B+=x2(i)*x2(i);
        }
        return delta*B;
    };
    while (true)
    {
        ite++;
        auto g=[&](vec& x)-> double {
            return f(x) + P(x);
        };
        x_fval result_;
        if(opt.algo == Powell)
        { result_= powell(g, x_);}
        else if(opt.algo == Rosenbrock)
        { result_= rosenbrock(g, x_);}
        x_=result_.x;
        if(P(x_) < eps || ite>=opt.max_ite)
        {
            x_fval result;
            result.x=x_;
            result.fval=f(x_);
            return result;
        }
        else
        {
            delta*=C;
        }
    }
}

x_fval sci_arma::fmincon(const obj_fun &f, vec &x0, mat &A, mat &b, mat &Aeq, mat &beq, const non_linear_con &c, const options& opt)
{
    try{
        if((int)x0.n_cols!=1)
            throw std::logic_error("ERROR: fmincon(): x0 must be a column vector.");
    }
    catch(std::exception& e){
        std::cerr<<e.what()<<std::endl;
        std::terminate();
    }
    umat com=A*x0-b < zeros(A.n_rows,1);
    int flag=0;
    for(auto i=0;i<A.n_rows;i++)
    {
        if(com(i,0)==0)
            flag++;
    }
    try{
        if(flag>0)
            throw std::logic_error("ERROR: fmincon(): x0 does not meet linear constraints, or on the boundary.");
    }
    catch(std::exception& e){
        std::cerr<<e.what()<<std::endl;
        std::terminate();
    }
    com=Aeq*x0-beq == zeros(Aeq.n_rows,1);
    flag=0;
    for(auto i=0;i<Aeq.n_rows;i++)
    {
        if(com(i,0)==0)
            flag++;
    }
    try{
        if(flag>0)
            throw std::logic_error("ERROR: fmincon(): x0 does not meet equality constraints.");
    }
    catch(std::exception& e){
        std::cerr<<e.what()<<std::endl;
        std::terminate();
    }
    double delta=1, C=1.1, eps=0.00001;
    long long ite=0;
    auto x_=x0;
    auto P=[&](vec& x)->double {
        vec x1=b-A*x, x2=beq-Aeq*x, x3=c(x);
        auto n_ineq=x1.n_rows;
        auto n_eq=x2.n_rows;
        auto n_non_lin=x3.n_rows;
        double B=0;
        for(auto i=0;i<n_ineq;i++)
        {
            double temp=std::max(0.0,-x1(i));
            B+=temp*temp;
        }
        for(auto i=0;i<n_eq;i++)
        {
            B+=x2(i)*x2(i);
        }
        for(auto i=0;i<n_non_lin;i++)
        {
            double temp=std::max(0.0,x3(i));
            B+=temp*temp;
        }
        return delta*B;
    };
    while (true)
    {
        ite++;
        auto g=[&](vec& x)-> double {
            return f(x) + P(x);
        };
        x_fval result_;
        if(opt.algo == Powell)
        { result_= powell(g, x_);}
        else if(opt.algo == Rosenbrock)
        { result_= rosenbrock(g, x_);}
        x_=result_.x;
        if(P(x_) < eps || ite>=opt.max_ite)
        {
            x_fval result;
            result.x=x_;
            result.fval=f(x_);
            return result;
        }
        else
        {
            delta*=C;
        }
    }
}


x_fval sci_arma::fmincon(const obj_fun& f, vec& x0, mat& A, mat& b)
{
    options opt;
    return fmincon(f, x0, A, b, opt);
}

x_fval sci_arma::fmincon(const obj_fun& f, vec& x0, mat& A, mat& b, mat& Aeq, mat& beq)
{
    options opt;
    return fmincon(f, x0, A, b, Aeq, beq, opt);
}

x_fval sci_arma::fmincon(const obj_fun& f, vec& x0, mat& A, mat& b, mat& Aeq, mat& beq, const non_linear_con& c)
{
    options opt;
    return fmincon(f, x0, A, b, Aeq, beq, c, opt);
}

//fmin with linear constraints
//'interior-point'
//this method is not well-implemented
/*
x_fval sci_arma::fmincon_i(const obj_fun &f, vec &x0, mat &A, mat &b)
{
    auto nvar=x0.n_rows;
    try{
        if((int)x0.n_cols!=1)
            throw std::logic_error("ERROR: x0 must be a column vector.");
    }
    catch(std::exception& e){
        std::cerr<<e.what()<<std::endl;
        std::terminate();
    }
    umat c=A*x0-b < zeros(nvar,1);
    int flag=0;
    for(auto i=0;i<nvar;i++)
    {
        if(c(i,0)==0)
            flag++;
    }
    try{
        if(flag>0)
            throw std::logic_error("ERROR: x0 is not in constraints, or on the boundary.");
    }
    catch(std::exception& e){
        std::cerr<<e.what()<<std::endl;
        std::terminate();
    }
    double r=1, beta=0.9, eps=0.0001;
    auto x_=x0;
    while (true)
    {
        auto g=[&](vec& x)-> double {
            return f(x)+r*std::abs(sum(ones(nvar,1)/(b-A*x)));
        };
        auto result_=fmin(g,x_);
        x_=result_.x;
        double temp=sum(ones(nvar,1)/(b-A*x_));
        if(r*std::abs(temp)<eps)
        {
            x_fval result;
            result.x=x_;
            result.fval=f(x_);
            return result;
        }
        else
        {
            r=beta*r;
        }
    }
}
*/